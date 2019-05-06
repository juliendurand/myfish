#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

#include <cstdint>

namespace chess{

    typedef uint64_t U64;
    typedef char Piece;

    struct Board{
        static const Piece EMPTY = ' ';
        static const Piece WHITE_PAWN = 'P';
        static const Piece WHITE_KNIGHT ='N';
        static const Piece WHITE_BISHOP ='B';
        static const Piece WHITE_ROOK ='R';
        static const Piece WHITE_QUEEN ='Q';
        static const Piece WHITE_KING ='K';
        static const Piece BLACK_PAWN ='p';
        static const Piece BLACK_KNIGHT ='n';
        static const Piece BLACK_BISHOP ='b';
        static const Piece BLACK_ROOK ='r';
        static const Piece BLACK_QUEEN = 'q';
        static const Piece BLACK_KING = 'k';

        static const int WHITE_PAWN_LAYER = 0;
        static const int WHITE_KNIGHT_LAYER = 1;
        static const int WHITE_BISHOP_LAYER = 2;
        static const int WHITE_ROOK_LAYER = 3;
        static const int WHITE_QUEEN_LAYER = 4;
        static const int WHITE_KING_LAYER = 5;
        static const int BLACK_PAWN_LAYER = 6;
        static const int BLACK_KNIGHT_LAYER = 7;
        static const int BLACK_BISHOP_LAYER = 8;
        static const int BLACK_ROOK_LAYER = 9;
        static const int BLACK_QUEEN_LAYER = 10;
        static const int BLACK_KING_LAYER = 11;

        static const int NB_LAYERS = 12;

        static const Piece PIECES[12];

        U64 board[NB_LAYERS];

        Board();
        void set_square(Piece piece, int square);
        Piece get_square(int square);
        void empty();
        void print();

    private:
        U64 get_bitmask(int square);
    };

    struct Position{
        static const U64 WHITE_KINGSIDE_CAST  = (1ULL << 0);
        static const U64 WHITE_QUEENSIDE_CAST = (1ULL << 1);
        static const U64 BLACK_KINGSIDE_CAST  = (1ULL << 2);
        static const U64 BLACK_QUEENSIDE_CAST = (1ULL << 3);

        U64 status;
        Board board;
        int plies;
        int reversible_plies;

        Position();
        std::string export_fen();
        void import_fen(const std::string &fen);
        void set_castling(Piece cast_type, bool right);
        bool get_castling(Piece cast_type);
        std::string get_en_passant();
        std::string get_all_castling();
        void reset();
    };

}

#endif // #ifndef POSITION_H_INCLUDED

