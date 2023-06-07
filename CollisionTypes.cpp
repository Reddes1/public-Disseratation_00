#include "CollisionTypes.h"

using namespace DirectX::SimpleMath;

bool BK_Collision::Rect2Rect(Vec4 lRect, Vec4 rRect)
{
    //Alter the rect positions for calculations (This moves them from origin @ center to Left/Top)
    lRect.x -= lRect.z * 0.5f;
    lRect.y -= lRect.w * 0.5f;

    rRect.x -= rRect.z * 0.5f;
    rRect.y -= rRect.w * 0.5f;

    //Check collision
    if (lRect.x + lRect.z >= rRect.x &&     //Rect1 Right edge past Rect2 Left
        lRect.x <= rRect.x + rRect.z &&     //Rect1 Left edge past Rect2 right
        lRect.y + lRect.w >= rRect.y &&     //Rect1 Top edge past Rect2 bottom
        lRect.y <= rRect.y + rRect.w)       //Rect1 bottom edge past Rect2 top
        return true;
    return false;
}

bool BK_Collision::Rect2Rect_Scaled(Vec4 lRect, Vec4 rRect, float lRectXScale, float lRectYScale, float rRectXScale, float rRectYScale)
{

    lRect.z *= lRectXScale;
    lRect.w *= lRectYScale;
    rRect.z *= rRectXScale;
    rRect.w *= rRectYScale;

    return Rect2Rect(lRect, rRect);
}

bool BK_Collision::Cir2Cir(const Vec4& lCircle, const Vec4& rCircle)
{

    float radiusL = (Vec2(lCircle.z, lCircle.w).Length() * 0.5f);  //Adjust to make the hitbox a little more generous(Move to interface later)
    float radiusR = (Vec2(rCircle.z, rCircle.w).Length() * 0.5f);

    float distance = Vector2{ lCircle.x - rCircle.x, lCircle.y - rCircle.y }.Length();

    //Check collision
    if (distance <= radiusL + radiusR)
        return true;
    return false;
}

bool BK_Collision::Cir2Cir_Scaled(Vec4 lCircle, Vec4 rCircle, float lCirjScale, float rCirScale)
{
    //Rescale left circle
    lCircle.z *= lCirjScale;
    lCircle.w *= lCirjScale;
    //Rescale right circle
    rCircle.z *= rCirScale;
    rCircle.w *= rCirScale;

    //Pass new values through collision function and return
    return (Cir2Cir(lCircle, rCircle));
}

bool BK_Collision::Cir2Rect(const Vec4& lCircle, Vec4 rRect)
{
    //Alter the rect position for calculations (This moves it from origin @ center to Left/Top)
    rRect.x -= rRect.z * 0.5f;
    rRect.y -= rRect.w * 0.5f;
    //Get raidus of cicle obj & make the hitbox a little generous (Move this into an interface option later).
    float radius = (Vector2{ lCircle.z, lCircle.w }.Length() / 2.f) * 0.7f;

    //Comparion containers
    float compX = lCircle.x;
    float compY = lCircle.y;

    //Check what edges we are closest to
    if (lCircle.x < rRect.x)                 //Left Edge
        compX = rRect.x;
    else if (lCircle.x > rRect.x + rRect.z)   //Right Edge
        compX = rRect.x + rRect.z;
    if (lCircle.y < rRect.y)                 //Top Edge
        compY = rRect.y;
    else if (lCircle.y > rRect.y + rRect.w)   //Bottom Edge
        compY = rRect.y + rRect.w;

    //Calculate distance
    float distance = Vector2{ lCircle.x - compX, lCircle.y - compY }.Length();

    //Check collision
    if (distance <= radius)
        return true;
    return false;
}

bool BK_Collision::Cir2Rect_Scaled(Vec4 lCircle, Vec4 rRect, float lCirScaling, float rRectScaleX, float rRectScaleY)
{
    //Rescale circle
    lCircle.z *= lCirScaling;
    lCircle.w *= lCirScaling;
    //Rescale rect
    rRect.z *= rRectScaleX;
    rRect.w *= rRectScaleY;

    //Pass new values through collision function and return
    return Cir2Rect(lCircle, rRect);
}

bool BK_Collision::Polygon2Point(std::vector<Vec2> vertices, Vec2 lPolyPos, Vec4 rRect)
{
    //Add current position given to shift the frame onto the sprite
    for (unsigned i(0); i < vertices.size(); ++i)
    {
        vertices[i] += lPolyPos;
    }

    //Flags and container for collision check
    bool collision = false;
    unsigned nextVertex = 0;

    //Run collision Check
    for (unsigned currentVertex(0); currentVertex < vertices.size(); ++currentVertex)
    {
        nextVertex = currentVertex + 1;
        if (nextVertex == vertices.size())
            nextVertex = 0;

        //Capture current and next vertex positions
        Vector2 vectCurrent = vertices[currentVertex];
        Vector2 vectNext = vertices[nextVertex];

        //Use mathematics (Jordan Curve Theorem) to check for collision
        if (((vectCurrent.y >= rRect.y && vectNext.y < rRect.y) ||
            (vectCurrent.y < rRect.y && vectNext.y >= rRect.y)) &&
            (rRect.x < (vectNext.x - vectCurrent.x) *
            (rRect.y - vectCurrent.y) / (vectNext.y - vectCurrent.y) + vectCurrent.x))
        {
            //Flip collision value
            collision = !collision;
        }
    }
    return collision;

}

bool BK_Collision::Line2Line(const Vec2& lLineStart, const Vec2& lLineEnd, const Vec2& rLineStart, const Vec2 rLineEnd)
{
    //Calulate direction of line1
    float uA = ((rLineEnd.x - rLineStart.x) * (lLineStart.y - rLineStart.y) - (rLineEnd.y - rLineStart.y) * (lLineStart.x - rLineStart.x)) /
        ((rLineEnd.y - rLineStart.y) * (lLineEnd.x - lLineStart.x) - (rLineEnd.x - rLineStart.x) * (lLineEnd.y - lLineStart.y));
    //Calulate direction of line2
    float uB = ((lLineEnd.x - lLineStart.x) * (lLineStart.y - rLineStart.y) - (lLineEnd.y - lLineStart.y) * (lLineStart.x - rLineStart.x))
        / ((rLineEnd.y - rLineStart.y) * (lLineEnd.x - lLineStart.x) - (rLineEnd.x - rLineStart.x) * (lLineEnd.y - lLineStart.y));

    //if uA and uB are between 0-1, they are colliding
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
        return true;
    return false;
}

bool BK_Collision::Line2Rect(const Vec2& lLineStart, Vec2& lLineEnd, Vec4 rRect)
{
    bool left = Line2Line(lLineStart, lLineEnd, Vec2(rRect.x, rRect.y), Vec2(rRect.x, rRect.y + rRect.w));
    bool right = Line2Line(lLineStart, lLineEnd, Vec2(rRect.x + rRect.z, rRect.y), Vec2(rRect.x + rRect.z, rRect.y + rRect.w));
    bool top = Line2Line(lLineStart, lLineEnd, Vec2(rRect.x, rRect.y), Vec2(rRect.x + rRect.z, rRect.y));
    bool bottom = Line2Line(lLineStart, lLineEnd, Vec2(rRect.x, rRect.y + rRect.w), Vec2(rRect.x + rRect.z, rRect.y + rRect.w));

    // if ANY of the above are true,
    // the line has hit the rectangle
    if (left || right || top || bottom)
        return true;
    return false;
}

bool BK_Collision::Polygon2Rect(std::vector<Vec2> vertices, Vec2 lPolyPos, Vec4 rRect, bool checkIfInside)
{

    //Alter the rect positions for calculations (This moves them from origin @ center to Left/Top)
    rRect.x -= rRect.z * 0.5f;
    rRect.y -= rRect.w * 0.5f;

    //Add current position given to shift the frame onto the sprite
    for (size_t i(0); i < vertices.size(); ++i)
    {
        vertices[i] += lPolyPos;
    }

    //Flags and container for collision check
    bool collision = false;
    int nextVertex = 0;

    //Run collision Check
    for (int currentVertex(0); currentVertex < vertices.size(); ++currentVertex)
    {
        nextVertex = currentVertex + 1;
        if (nextVertex == vertices.size())
            nextVertex = 0;

        //Capture current and next vertex positions
        Vector2 vectCurrent = vertices[currentVertex];
        Vector2 vectNext = vertices[nextVertex];
        // check against all four sides of the rectangle
        collision = Line2Rect(vectCurrent, vectNext, rRect);
        if (collision)
            return true;


        //option check to see if the rect is inside the polygon for collision purposes
        if (checkIfInside)
        {
            bool inside = Polygon2Point(vertices, lPolyPos, rRect);
            if (inside)
                return true;
        }
    }

    return false;
}
