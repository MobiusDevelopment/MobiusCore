/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SERVER_AREA_BOUNDARY_H
#define SERVER_AREA_BOUNDARY_H

#include "Position.h"

class GAME_API AreaBoundary
{
    public:
        enum BoundaryType
        {
            BOUNDARY_RECTANGLE, // Rectangle aligned with the coordinate axis
            BOUNDARY_CIRCLE,
            BOUNDARY_ELLIPSE,
            BOUNDARY_TRIANGLE,
            BOUNDARY_PARALLELOGRAM,
            BOUNDARY_Z_RANGE,
        };
        virtual ~AreaBoundary() { }
        BoundaryType GetBoundaryType() const { return m_boundaryType; }
        bool IsWithinBoundary(Position const* pos) const { return (IsWithinBoundaryArea(pos) != m_isInvertedBoundary); }
        bool IsWithinBoundary(Position const& pos) const { return IsWithinBoundary(&pos); }

        struct DoublePosition : Position
        {
            double d_positionX, d_positionY, d_positionZ;
            DoublePosition(double x = 0.0, double y = 0.0, double z = 0.0, float o = 0.0f)
                : Position(float(x), float(y), float(z), o), d_positionX(x), d_positionY(y), d_positionZ(z) { }
            DoublePosition(float x, float y = 0.0f, float z = 0.0f, float o = 0.0f)
                : Position(x, y, z, o), d_positionX(x), d_positionY(y), d_positionZ(z) { }
            DoublePosition(Position const & pos)
                : DoublePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()) { }

            double GetDoublePositionX() const { return d_positionX; }
            double GetDoublePositionY() const { return d_positionY; }
            double GetDoublePositionZ() const { return d_positionZ; }

            double GetDoubleExactDist2dSq(DoublePosition const& pos) const {
                double offX = GetDoublePositionX() - pos.GetDoublePositionX();
                double offY = GetDoublePositionY() - pos.GetDoublePositionY();
                return (offX*offX) + (offY*offY);
            }

            Position* sync() { m_positionX = (float)d_positionX; m_positionY = (float)d_positionY; m_positionZ = (float)d_positionZ; return this; }
        };

    protected:
        AreaBoundary(BoundaryType bType, bool isInverted) : m_boundaryType(bType), m_isInvertedBoundary(isInverted) { }
        virtual bool IsWithinBoundaryArea(Position const* pos) const = 0;
        const BoundaryType m_boundaryType;
        bool m_isInvertedBoundary;
};

class GAME_API RectangleBoundary : public AreaBoundary
{
    public:
        // X axis is north/south, Y axis is east/west, larger values are northwest
        RectangleBoundary(float southX, float northX, float eastY, float westY, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const float _minX, _maxX, _minY, _maxY;
};

class GAME_API CircleBoundary : public AreaBoundary
{
    public:
        CircleBoundary(Position const& center, double radius, bool isInverted = false);
        CircleBoundary(DoublePosition const& center, double radius, bool isInverted = false);
        CircleBoundary(Position const& center, Position const& pointOnCircle, bool isInverted = false);
        CircleBoundary(DoublePosition const& center, DoublePosition const& pointOnCircle, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const DoublePosition _center;
        const double _radiusSq;
};

class GAME_API EllipseBoundary : public AreaBoundary
{
    public:
        EllipseBoundary(Position const& center, double radiusX, double radiusY, bool isInverted = false);
        EllipseBoundary(DoublePosition const& center, double radiusX, double radiusY, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const DoublePosition _center;
        const double _radiusYSq, _scaleXSq;
};

class GAME_API TriangleBoundary : public AreaBoundary
{
    public:
        TriangleBoundary(Position const& pointA, Position const& pointB, Position const& pointC, bool isInverted = false);
        TriangleBoundary(DoublePosition const& pointA, DoublePosition const& pointB, DoublePosition const& pointC, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const DoublePosition _a, _b, _c;
        const double _abx, _bcx, _cax, _aby, _bcy, _cay;
};

class GAME_API ParallelogramBoundary : public AreaBoundary
{
    public:
        // Note: AB must be orthogonal to AD
        ParallelogramBoundary(Position const& cornerA, Position const& cornerB, Position const& cornerD, bool isInverted = false);
        ParallelogramBoundary(DoublePosition const& cornerA, DoublePosition const& cornerB, DoublePosition const& cornerD, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const DoublePosition _a, _b, _d, _c;
        const double _abx, _dax, _aby, _day;
};

class GAME_API ZRangeBoundary : public AreaBoundary
{
    public:
        ZRangeBoundary(float minZ, float maxZ, bool isInverted = false);

    protected:
        bool IsWithinBoundaryArea(Position const* pos) const override;

    private:
        const float _minZ, _maxZ;
};

#endif //SERVER_AREA_BOUNDARY_H
