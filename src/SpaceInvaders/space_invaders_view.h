#pragma once

class SpaceInvadersView
{
public:
    virtual void init() = 0;

    virtual bool render() = 0;

    virtual ~SpaceInvadersView() {}
};