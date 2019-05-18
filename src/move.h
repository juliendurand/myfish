#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <string>
#include <vector>

#include "position.h"
#include "utils.h"

namespace chess {

    U64 perft(int depth, Position* position, bool printinfo);

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
                 U8 en_passant);
        U8 get_from_layer();
        Piece get_from_piece();
        U8 get_from_square();
        U8 get_to_layer();
        Piece get_to_piece();
        U8 get_to_square();
        U8 get_take_square();
        U8 get_en_passant();
        bool is_promotion();
        std::string to_long_algebraic();
    };

    class MoveGenerator{
    private:
        Position* position;
        //Move move;
        //Board* to_board;
        int turn;
        int opponent;
        int layer;
        U64 square_mask;
        int seq;
        U64 own_pieces;
        U64 opponent_pieces;
        U64 all_pieces;
        U64 free_square;

    public:
        std::vector<Move> moveList;

        MoveGenerator(Position* pos);
        int generate();
        bool ischeck(Move* m);
        U64 generate_attacks(U64 to, U64 notOpponentPieces, U64 free_square);
        void generate_move_bitscan(int layer, int from, U64 bits, U8 en_passant = 0);
        U64 generate_pawn_pushes(U64 layer, Color color, U64 free_square);
        U64 generate_pawn_double_pushes(U64 layer, Color color, U64 free_square);
        U64 generate_pawn_attacks(U64 layer, Color color, U64 notSelf);
        U64 generate_knight_attacks(U64 layer, U64 notSelf);
        U64 generate_bishop_attacks(U64 layer, U64 notSelf, U64 free_square);
        U64 generate_rook_attacks(U64 layer, U64 notSelf, U64 free_square);
        U64 generate_queen_attacks(U64 layer, U64 notSelf, U64 free_square);
        U64 generate_king_attacks(U64 layer, U64 notSelf);
        void generate_castling();
        U64 expandN(U64 layer, U64 notSelf, U64 free_square);
        U64 expandNE(U64 layer, U64 notSelf, U64 free_square);
        U64 expandE(U64 layer, U64 notSelf, U64 free_square);
        U64 expandSE(U64 layer, U64 notSelf, U64 free_square);
        U64 expandS(U64 layer, U64 notSelf, U64 free_square);
        U64 expandSW(U64 layer, U64 notSelf, U64 free_square);
        U64 expandW(U64 layer, U64 notSelf, U64 free_square);
        U64 expandNW(U64 layer, U64 notSelf, U64 free_square);
    };

}

#endif // #ifndef MOVE_H_INCLUDED
