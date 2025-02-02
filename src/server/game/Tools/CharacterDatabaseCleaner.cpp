/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "Common.h"
#include "CriteriaHandler.h"
#include "CharacterDatabaseCleaner.h"
#include "DB2Stores.h"
#include "Log.h"
#include "World.h"
#include "Database/DatabaseEnv.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include <sstream>

void CharacterDatabaseCleaner::CleanDatabase()
{
    // config to disable
    if (!sWorld->getBoolConfig(CONFIG_CLEAN_CHARACTER_DB))
        return;

    LOG_INFO("misc", "Cleaning character database...");

    uint32 oldMSTime = getMSTime();

    // check flags which clean ups are necessary
    QueryResult result = CharacterDatabase.PQuery("SELECT value FROM worldstates WHERE entry = %d", WS_CLEANING_FLAGS);
    if (!result)
        return;

    uint32 flags = (*result)[0].GetUInt32();

    // clean up
    if (flags & CLEANING_FLAG_ACHIEVEMENT_PROGRESS)
        CleanCharacterAchievementProgress();

    if (flags & CLEANING_FLAG_SKILLS)
        CleanCharacterSkills();

    if (flags & CLEANING_FLAG_SPELLS)
        CleanCharacterSpell();

    if (flags & CLEANING_FLAG_TALENTS)
        CleanCharacterTalent();

    if (flags & CLEANING_FLAG_QUESTSTATUS)
        CleanCharacterQuestStatus();

    // NOTE: In order to have persistentFlags be set in worldstates for the next cleanup,
    // you need to define them at least once in worldstates.
    flags &= sWorld->getIntConfig(CONFIG_PERSISTENT_CHARACTER_CLEAN_FLAGS);
    CharacterDatabase.DirectPExecute("UPDATE worldstates SET value = %u WHERE entry = %d", flags, WS_CLEANING_FLAGS);

    sWorld->SetCleaningFlags(flags);

    LOG_INFO("server.loading", ">> Cleaned character database in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void CharacterDatabaseCleaner::CheckUnique(const char* column, const char* table, bool (*check)(uint32))
{
    QueryResult result = CharacterDatabase.PQuery("SELECT DISTINCT %s FROM %s", column, table);
    if (!result)
    {
        LOG_INFO("misc", "Table %s is empty.", table);
        return;
    }

    bool found = false;
    std::ostringstream ss;
    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        if (!check(id))
        {
            if (!found)
            {
                ss << "DELETE FROM " << table << " WHERE " << column << " IN (";
                found = true;
            }
            else
                ss << ',';

            ss << id;
        }
    }
    while (result->NextRow());

    if (found)
    {
        ss << ')';
        CharacterDatabase.Execute(ss.str().c_str());
    }
}

bool CharacterDatabaseCleaner::AchievementProgressCheck(uint32 criteria)
{
    return sCriteriaMgr->GetCriteria(criteria) != nullptr;
}

void CharacterDatabaseCleaner::CleanCharacterAchievementProgress()
{
    CheckUnique("criteria", "character_achievement_progress", &AchievementProgressCheck);
}

bool CharacterDatabaseCleaner::SkillCheck(uint32 skill)
{
    return sSkillLineStore.LookupEntry(skill) != nullptr;
}

void CharacterDatabaseCleaner::CleanCharacterSkills()
{
    CheckUnique("skill", "character_skills", &SkillCheck);
}

bool CharacterDatabaseCleaner::SpellCheck(uint32 spell_id)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
    return spellInfo && !spellInfo->HasAttribute(SPELL_ATTR0_CU_IS_TALENT);
}

void CharacterDatabaseCleaner::CleanCharacterSpell()
{
    CheckUnique("spell", "character_spell", &SpellCheck);
}

bool CharacterDatabaseCleaner::TalentCheck(uint32 talent_id)
{
    TalentEntry const* talentInfo = sTalentStore.LookupEntry(talent_id);
    if (!talentInfo)
        return false;

    return sChrSpecializationStore.LookupEntry(talentInfo->SpecID) != nullptr;
}

void CharacterDatabaseCleaner::CleanCharacterTalent()
{
    CharacterDatabase.DirectPExecute("DELETE FROM character_talent WHERE talentGroup > %u", MAX_SPECIALIZATIONS);
    CheckUnique("talentId", "character_talent", &TalentCheck);
}

void CharacterDatabaseCleaner::CleanCharacterQuestStatus()
{
    CharacterDatabase.DirectExecute("DELETE FROM character_queststatus WHERE status = 0");
}
