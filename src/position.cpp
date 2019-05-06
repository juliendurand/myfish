#include <string>

#include "position.h"

namespace chess {

    void Board::set_square(char piece, int file, int rank){
        int bit_position = (file - 'a') + (rank - 1) * 8;
        U64 bitmask = (1 << bit_position);
        for(int layer = 0; layer < NB_LAYERS; layer++){
            board[layer] &= ~bitmask;
        }
        int layer = -1;
        switch(piece){
            case ' ':
                return;
            case 'P':
                layer = WHITE_PAWN;
                break;
            case 'N':
                layer = WHITE_KNIGHT;
                break;
            case 'B':
                layer = WHITE_BISHOP;
                break;
            case 'R':
                layer = WHITE_ROOK;
                break;
            case 'Q':
                layer = WHITE_QUEEN;
                break;
            case 'K':
                layer = WHITE_KING;
                break;
            case 'p':
                layer = BLACK_PAWN;
                break;
            case 'n':
                layer = BLACK_KNIGHT;
                break;
            case 'b':
                layer = BLACK_BISHOP;
                break;
            case 'r':
                layer = BLACK_ROOK;
                break;
            case 'q':
                layer = BLACK_QUEEN;
                break;
            case 'k':
                layer = BLACK_KING;
                break;
            default:
                return; // TODO : generate Exception ?
        }
        board[layer] |= bitmask;
    }

    void Board::empty(){
        for(int layer = 0; layer < NB_LAYERS; layer++){
            board[layer] = 0;
        }
    }

    std::string Position::export_fen(){
        return "";
    }

    void Position::import_fen(const std::string &fen){
    }
}
