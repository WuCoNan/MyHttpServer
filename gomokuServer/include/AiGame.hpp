#pragma once
#include <string>
#include <vector>
class AiGame
{
public:
    explicit AiGame(const std::string& userId);
    bool playerMove(int x,int y);
    void aiMove();
    bool isPlayerWinner() const;
    bool isAiWinner() const;
    bool isDraw() const;
    void resetGame();
    std::pair<int,int> getLastAiMove() const { return lastAiMove_; };
    static constexpr std::size_t BOARD_SIZE =15;
private:
    enum class BoardState
    {
        EMPTY,
        PLAYER,
        AI
    };
    enum class GameState
    {
        ONGOING,
        PLAYER_WON,
        AI_WON,
        DRAW
    };
    void checkGameState(int x,int y);
    bool checkHorizontal(int x,int y) const;
    bool checkVertical(int x,int y) const;
    bool checkDiagonal(int x,int y) const;
    bool checkAntiDiagonal(int x,int y) const;
    bool checkDraw() const;
    std::string userId_;
    std::vector<std::vector<BoardState>> board_;
    GameState gameState_ = GameState::ONGOING;
    std::pair<int,int> lastAiMove_{-1,-1};
};