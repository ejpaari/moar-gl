bool isTransparent(float alpha)
{
    return alpha < 0.1;
}

bool isTooFar(float lightPower)
{
    return lightPower < 0.05;
}