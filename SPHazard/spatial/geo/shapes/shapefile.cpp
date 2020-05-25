#include "shapealgorithms.h"
#include "shapefile.h"

ShapeFile * ShapeFile::AS_And(ShapeFile * other)
{
    ShapeLayer * l1 = GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,SPH_SHAPE_INTERSECT);
}

ShapeFile * ShapeFile::AS_Or(ShapeFile * other)
{
    ShapeLayer * l1 = GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,SPH_SHAPE_UNION);
}

ShapeFile * ShapeFile::AS_Xor(ShapeFile * other)
{
    ShapeLayer * l1 = GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,SPH_SHAPE_XOR);
}

ShapeFile * ShapeFile::AS_Add(ShapeFile * other)
{
    ShapeLayer * l1 = GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,SPH_SHAPE_ADD);
}

ShapeFile * ShapeFile::AS_Sub(ShapeFile * other)
{
    ShapeLayer * l1 = GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,SPH_SHAPE_DIFFERENCE);
}
