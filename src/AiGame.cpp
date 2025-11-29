#include "AiGame.hpp"

AiGame::AiGame(const std::string& userId)
    : userId_(userId)
    , board_(BOARD_SIZE, std::vector<BoardState>(BOARD_SIZE, BoardState::EMPTY))
{
}

void AiGame::resetGame()
{
    for (auto& row:board_)
    {
        std::fill(row.begin(),row.end(),BoardState::EMPTY);
    }
}

bool AiGame::isPlayerWinner() const
{
    return gameState_ == GameState::PLAYER_WON;
}

bool AiGame::isAiWinner() const
{
    return gameState_ == GameState::AI_WON;
}

bool AiGame::isDraw() const
{
    return gameState_ == GameState::DRAW;
}

bool AiGame::playerMove(int x, int y)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board_[x][y] != BoardState::EMPTY)
    {
        return false;  // Invalid move
    }

    board_[x][y] = BoardState::PLAYER;
    checkGameState(x,y);
    return true;
}

void AiGame::aiMove()
{
    // Simple AI: choose the first available spot
    for (std::size_t i = 0; i < BOARD_SIZE; ++i)
    {
        for (std::size_t j = 0; j < BOARD_SIZE; ++j)
        {
            if (board_[i][j] == BoardState::EMPTY)
            {
                board_[i][j] = BoardState::AI;
                lastAiMove_ = {static_cast<int>(i), static_cast<int>(j)};
                checkGameState(i,j);
                return;
            }
        }
    }
}

void AiGame::checkGameState(int x,int y)
{
    // Check for winning conditions or draw
    // Update gameState_ accordingly
    if(checkHorizontal(x,y)||
       checkVertical(x,y)||
       checkDiagonal(x,y)||
       checkAntiDiagonal(x,y))
    {
        gameState_ = (board_[x][y]==BoardState::PLAYER)?GameState::PLAYER_WON:GameState::AI_WON;
    }
    else if(checkDraw())
    {
        gameState_ = GameState::DRAW;
    }
}

bool AiGame::checkHorizontal(int x,int y) const
{
    // Check horizontal win condition
    int left=std::max(0,x-4),right=left+5;
    int count=0;
    for(int i=left;i<right;++i)
    {
        if(board_[i][y]==board_[x][y])
            ++count;
    }
    while(left<=x&&right<BOARD_SIZE)
    {
        if(count==5)
            return true;
        if(board_[left][y]==board_[x][y])
            --count;
        if(board_[right][y]==board_[x][y])
            ++count;
        ++left;
        ++right;
    }
    return count==5;
}

bool AiGame::checkVertical(int x,int y) const
{
    // Check vertical win condition
    int down=std::max(0,y-4),up=down+5;
    int count=0;
    for(int i=down;i<up;++i)
    {
        if(board_[x][i]==board_[x][y])
            ++count;
    }
    while(down<=y&&up<BOARD_SIZE)
    {
        if(count==5)
            return true;
        if(board_[x][down]==board_[x][y])
            --count;
        if(board_[x][up]==board_[x][y])
            ++count;
        ++down;
        ++up;
    }
    return count==5;
}

bool AiGame::checkDiagonal(int x, int y) const
{
    auto currentPlayer = board_[x][y];
    if (currentPlayer == BoardState::EMPTY) return false;

    // Find the starting point of the diagonal line that contains (x,y)
    int startX = x;
    int startY = y;
    
    // Move to the top-left end of the line
    while (startX > 0 && startY > 0 && board_[startX - 1][startY - 1] == currentPlayer) {
        startX--;
        startY--;
    }
    
    // Count consecutive pieces in diagonal direction (bottom-right)
    int count = 0;
    int curX = startX;
    int curY = startY;
    
    while (curX < BOARD_SIZE && curY < BOARD_SIZE && count < 5) {
        if (board_[curX][curY] == currentPlayer) {
            count++;
            if (count == 5) return true;
        } else {
            count = 0; // Reset count if sequence breaks
        }
        curX++;
        curY++;
    }
    
    return false;
}
bool AiGame::checkAntiDiagonal(int x, int y) const
{
    // Check anti-diagonal (top-right to bottom-left) win condition
    auto currentPlayer = board_[x][y];
    if (currentPlayer == BoardState::EMPTY) return false;

    // Find the starting point of the anti-diagonal line that contains (x,y)
    int startX = x;
    int startY = y;
    
    // Move to the top-right end of the line
    while (startX > 0 && startY < BOARD_SIZE - 1 && board_[startX - 1][startY + 1] == currentPlayer) {
        startX--;
        startY++;
    }
    
    // Count consecutive pieces in anti-diagonal direction (bottom-left)
    int count = 0;
    int curX = startX;
    int curY = startY;
    
    while (curX < BOARD_SIZE && curY >= 0 && count < 5) {
        if (board_[curX][curY] == currentPlayer) {
            count++;
            if (count == 5) return true;
        } else {
            count = 0; // Reset count if sequence breaks
        }
        curX++;
        curY--;
    }
    
    return false;
}

bool AiGame::checkDraw() const
{
    for (const auto& row : board_)
    {
        for (const auto& cell : row)
        {
            if (cell == BoardState::EMPTY)
            {
                return false;  // Found an empty cell, not a draw
            }
        }
    }
    return true;  // No empty cells, it's a draw
}