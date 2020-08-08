#pragma once
#include "EnumParser.h"

enum class Channel {
	Pawn, OverlapAll, Static, UI, Player
};

EnumParser<Channel>::EnumParser()
{
    enumMap["Pawn"] = Channel::Pawn;
    enumMap["OverlapAll"] = Channel::OverlapAll;
    enumMap["Static"] = Channel::Static;
    enumMap["UI"] = Channel::UI;
    enumMap["Player"] = Channel::Player;
}
