/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "TCSoap.h"
#include "soapH.h"
#include "soapStub.h"
#include "World.h"
#include "AccountMgr.h"
#include "Log.h"

void TCSoapThread(const std::string& host, uint16 port)
{
    struct soap soap;
    soap_init(&soap);
    soap_set_imode(&soap, SOAP_C_UTFSTRING);
    soap_set_omode(&soap, SOAP_C_UTFSTRING);

    // check every 3 seconds if world ended
    soap.accept_timeout = 3;
    soap.recv_timeout = 5;
    soap.send_timeout = 5;
    if (!soap_valid_socket(soap_bind(&soap, host.c_str(), port, 100)))
    {
        LOG_ERROR("network.soap", "Couldn't bind to %s:%d", host.c_str(), port);
        exit(-1);
    }

    LOG_INFO("network.soap", "Bound to http://%s:%d", host.c_str(), port);

    while (!World::IsStopped())
    {
        if (!soap_valid_socket(soap_accept(&soap)))
            continue;   // ran into an accept timeout

        LOG_DEBUG("network.soap", "Accepted connection from IP=%d.%d.%d.%d", (int)(soap.ip>>24)&0xFF, (int)(soap.ip>>16)&0xFF, (int)(soap.ip>>8)&0xFF, (int)soap.ip&0xFF);
        struct soap* thread_soap = soap_copy(&soap);// make a safe copy
        process_message(thread_soap);
    }

    soap_done(&soap);
}

void process_message(struct soap* soap_message)
{
    LOG_TRACE("network.soap", "SOAPWorkingThread::process_message");

    soap_serve(soap_message);
    soap_destroy(soap_message); // dealloc C++ data
    soap_end(soap_message); // dealloc data and clean up
    soap_done(soap_message); // detach soap struct
    free(soap_message);
}
/*
Code used for generating stubs:

int ns1__executeCommand(char* command, char** result);
*/
int ns1__executeCommand(soap* soap, char* command, char** result)
{
    // security check
    if (!soap->userid || !soap->passwd)
    {
        LOG_INFO("network.soap", "Client didn't provide login information");
        return 401;
    }

    uint32 accountId = AccountMgr::GetId(soap->userid);
    if (!accountId)
    {
        LOG_INFO("network.soap", "Client used invalid username '%s'", soap->userid);
        return 401;
    }

    if (!AccountMgr::CheckPassword(accountId, soap->passwd))
    {
        LOG_INFO("network.soap", "Invalid password for account '%s'", soap->userid);
        return 401;
    }

    if (AccountMgr::GetSecurity(accountId) < SEC_ADMINISTRATOR)
    {
        LOG_INFO("network.soap", "%s's gmlevel is too low", soap->userid);
        return 403;
    }

    if (!command || !*command)
        return soap_sender_fault(soap, "Command can not be empty", "The supplied command was an empty string");

    LOG_INFO("network.soap", "Received command '%s'", command);
    SOAPCommand connection;

    // commands are executed in the world thread. We have to wait for them to be completed
    {
        // CliCommandHolder will be deleted from world, accessing after queueing is NOT safe
        CliCommandHolder* cmd = new CliCommandHolder(&connection, command, &SOAPCommand::print, &SOAPCommand::commandFinished);
        sWorld->QueueCliCommand(cmd);
    }

    // Wait until the command has finished executing
    connection.finishedPromise.get_future().wait();

    // The command has finished executing already
    char* printBuffer = soap_strdup(soap, connection.m_printBuffer.c_str());
    if (connection.hasCommandSucceeded())
    {
        *result = printBuffer;
        return SOAP_OK;
    }
    else
        return soap_sender_fault(soap, printBuffer, printBuffer);
}

void SOAPCommand::commandFinished(void* soapconnection, bool success)
{
    SOAPCommand* con = (SOAPCommand*)soapconnection;
    con->setCommandSuccess(success);
}

////////////////////////////////////////////////////////////////////////////////
//
//  Namespace Definition Table
//
////////////////////////////////////////////////////////////////////////////////

struct Namespace namespaces[] =
{   { "SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", NULL, NULL }, // must be first
    { "SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", NULL, NULL }, // must be second
    { "xsi", "http://www.w3.org/1999/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
    { "xsd", "http://www.w3.org/1999/XMLSchema",          "http://www.w3.org/*/XMLSchema", NULL },
    { "ns1", "urn:TC", NULL, NULL },     // "ns1" namespace prefix
    { NULL, NULL, NULL, NULL }
};
