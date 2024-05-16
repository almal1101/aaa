#pragma once

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <climits>
#include <iomanip>
#include <iostream>

#include "Blackboard.h"
#include "Game.h"
#include "Player.h"

class AIPlayer : public Player
{
public:
    explicit AIPlayer(PlayerType ecPlayerType) : Player(ecPlayerType) {};

    ~AIPlayer() {};

    virtual void Initialize(std::string sHost, int nPort, bool& bSwap) override { (void)sHost; (void)nPort; (void)bSwap; };

    virtual bool Move(Game& cGame) override;

    virtual bool Finish(Game& cGame) override { (void)cGame; return true; }

    std::string TypeName() const override { return "AIPlayer"; }

private:
    GameMove MinimaxMove(int nPlayer, Game& cGame, int nDepth);

    int MinMove(int nPlayer, Game& cGame, int nDepth, int nAlpha, int nBeta);
    int MaxMove(int nPlayer, Game& cGame, int nDepth, int nAlpha, int nBeta);

    Blackboard m_cBlackBoard;
};

#endif // AIPLAYER_H