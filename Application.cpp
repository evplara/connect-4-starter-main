#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/Connect4.h"


namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                }
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Connect 4")) {
                        game = new Connect4(); 
                        game->setUpBoard(); }
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());
                    if (game && game->gameHasAI()) {
                        ImGui::Separator();
                        ImGui::TextUnformatted("Mode");

                        static int mode = 0; 
                        int prevMode = mode;

                        // Render buttons
                        ImGui::RadioButton("Player vs Player", &mode, 0); ImGui::SameLine();
                        ImGui::RadioButton("Human vs AI (AI = P1)", &mode, 1); ImGui::SameLine();
                        ImGui::RadioButton("Human vs AI (AI = P2)", &mode, 2); ImGui::SameLine();
                        ImGui::RadioButton("AI vs AI", &mode, 3);

                        if (mode != prevMode){
                            if (mode == 0) { // PvP
                                game->getPlayerAt(0)->setAIPlayer(false);
                                game->getPlayerAt(1)->setAIPlayer(false);
                                game->_gameOptions.AIvsAI = false;
                            } else if (mode == 1) { // AI = Player 1
                                game->getPlayerAt(0)->setAIPlayer(true);
                                game->getPlayerAt(1)->setAIPlayer(false);
                                game->_gameOptions.AIvsAI = false;
                            } else if (mode == 2) { // AI = Player 2
                                game->getPlayerAt(0)->setAIPlayer(false);
                                game->getPlayerAt(1)->setAIPlayer(true);
                                game->_gameOptions.AIvsAI = false;
                            } else { // AI vs AI
                                game->getPlayerAt(0)->setAIPlayer(true);
                                game->getPlayerAt(1)->setAIPlayer(true);
                                game->_gameOptions.AIvsAI = true;
                            }
                            gameOver = false;
                            gameWinner = -1;
                            game->stopGame();
                            game->setUpBoard();
                        }
                    }
                }    
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    if (game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
