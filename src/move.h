#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <string>

#include "position.h"
#include "utils.h"

namespace chess {

    class Move{
    private:
        U8 from_layer;  // 4 bits
        U8 from_square; // 6 bits
        U8 to_layer;    // 4 bits
        U8 to_square;   // 6 bits
        U8 take_square;        // 6 bits
        U8 en_passant;  // 6 bits

    public:
        void set(U8 from_layer, U8 from_square, U8 to_layer, U8 to_square,
                 U8 take_square, U8 en_passant);
        U8 get_from_layer();
        Piece get_from_piece();
        U8 get_from_square();
        U8 get_to_layer();
        Piece get_to_piece();
        U8 get_to_square();
        U8 get_take_square();
        U8 get_en_passant();
        bool is_promotion();
        void from_long_algebraic(const std::string &m, Board* board);
        std::string to_long_algebraic();
        void from_san(const std::string &m, Board* board);
        std::string to_san();
    };

    struct MoveGenerator{
    private:
        Position* position;
        Move *move;
        Board* to_board;
        int turn;
        int layer;
        U64 square_mask;
        int seq;
        U64 own_pieces;
        U64 opponent_pieces;
        int king_square;

    public:
        MoveGenerator(Position* pos);
        bool next(Move *m);
        bool ischeck();
        bool next_pseudo_legal();
        bool next_white_pawn();
        bool next_knight();
        bool next_bishop();
        bool next_rook();
        bool next_queen();
        bool next_white_king();
        bool next_black_king();
    };

}

#endif // #ifndef MOVE_H_INCLUDED
