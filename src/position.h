#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

#include <cstdint>

namespace chess{

    typedef uint_fast64_t U64;

    struct Board{
        static const int WHITE_PAWN = 0;
        static const int WHITE_KNIGHT = 1;
        static const int WHITE_BISHOP = 2;
        static const int WHITE_ROOK = 3;
        static const int WHITE_QUEEN = 4;
        static const int WHITE_KING = 5;
        static const int BLACK_PAWN = 6;
        static const int BLACK_KNIGHT = 7;
        static const int BLACK_BISHOP = 8;
        static const int BLACK_ROOK = 9;
        static const int BLACK_QUEEN = 10;
        static const int BLACK_KING = 11;
        static const int NB_LAYERS = 12;

        U64 board[NB_LAYERS];

        void set_square(char piece, int file, int rank);
        void empty();
    };

    struct Position{
        static const U64 WHITE_KINGSIDE_CAST  = (1 << 0);
        static const U64 WHITE_QUEENSIDE_CAST = (1 << 1);
        static const U64 BLACK_KINGSIDE_CAST  = (1 << 2);
        static const U64 BLACK_QUEENSIDE_CAST = (1 << 3);

        U64 status;
        Board board;
        int plies;


        std::string export_fen();
        void import_fen(const std::string &fen);
    };

}

#endif // #ifndef POSITION_H_INCLUDED

