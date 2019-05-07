#include <bitset>
#include <iostream>
#include <sstream>
#include <string>

#include "position.h"

namespace chess {

    const Piece Board::PIECES[] = {
            Board::WHITE_PAWN, Board::WHITE_KNIGHT, Board::WHITE_BISHOP,
            Board::WHITE_ROOK, Board::WHITE_QUEEN, Board::WHITE_KING,
            Board::BLACK_PAWN, Board::BLACK_KNIGHT, Board::BLACK_BISHOP,
            Board::BLACK_ROOK, Board::BLACK_QUEEN, Board::BLACK_KING
        };

    int coordinates_to_square(char file, char rank){
        if (file < 'a' || file > 'h' || rank < '1' || rank > '8'){
            return -1; // TODO throw Exception ?
        }
        return (file - 'a') + (rank - 1) * 8;
    }

    std::string square_to_coordinate(int square){
        std::string  file(1, char('a' + square % 8));
        std::string rank(1, char('1' + square / 8));
        return file + rank;
    }

    Board::Board(){
        empty();
    }

    U64 Board::get_bitmask(int square){
        return U64(1) << square;
    }

    void Board::set_square(Piece piece, int square){
        U64 bitmask = get_bitmask(square);
        for(int layer = 0; layer < NB_LAYERS; layer++){
            board[layer] &= ~bitmask;
        }
        int layer = -1;
        switch(piece){
            case EMPTY:
                return;
            case WHITE_PAWN:
                layer = WHITE_PAWN_LAYER;
                break;
            case WHITE_KNIGHT:
                layer = WHITE_KNIGHT_LAYER;
                break;
            case WHITE_BISHOP:
                layer = WHITE_BISHOP_LAYER;
                break;
            case WHITE_ROOK:
                layer = WHITE_ROOK_LAYER;
                break;
            case WHITE_QUEEN:
                layer = WHITE_QUEEN_LAYER;
                break;
            case WHITE_KING:
                layer = WHITE_KING_LAYER;
                break;
            case BLACK_PAWN:
                layer = BLACK_PAWN_LAYER;
                break;
            case BLACK_KNIGHT:
                layer = BLACK_KNIGHT_LAYER;
                break;
            case BLACK_BISHOP:
                layer = BLACK_BISHOP_LAYER;
                break;
            case BLACK_ROOK:
                layer = BLACK_ROOK_LAYER;
                break;
            case BLACK_QUEEN:
                layer = BLACK_QUEEN_LAYER;
                break;
            case BLACK_KING:
                layer = BLACK_KING_LAYER;
                break;
            default:
                return; // TODO : generate Exception ?
        }
        board[layer] |= bitmask;
    }

    Piece Board::get_square(int square){
        U64 bitmask = get_bitmask(square);
        for(int l = 0; l < NB_LAYERS; l++){
            if(board[l] & bitmask){
                return chess::Board::PIECES[l];
            }
        }
        return EMPTY;
    }

    void Board::empty(){
        for(int layer = 0; layer < NB_LAYERS; layer++){
            board[layer] = 0;
        }
    }

    void Board::print(){
        for(int l = 0; l < NB_LAYERS; l++){
            std::bitset<64> bs(board[l]);
            std::cout << bs << std::endl;
        }
    }

    Position::Position() : board(Board()){
        reset();
        const std::string START_POSITION =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        import_fen(START_POSITION);
    }

    std::string Position::export_fen(){
        // TODO
        return "";
    }

    void Position::import_fen(const std::string &fen){
        reset();
        char sep = ' ';
        std::stringstream ss = std::stringstream(fen);
        std::string token;

        // board
        getline(ss, token, sep);
        int square = 56;
        for(char c : token){
            if(c >= '1' && c <= '8'){
                square += c - '0'; // n empty squares
                continue;
            }
            if(c == '/'){
                square -= 16;
                continue;
            }
            board.set_square(c, square);
            square++;
        }

        // turn
        getline(ss, token, sep);
        int turn = (token == "b") ? 1 : 0;

        // castling rights
        getline(ss, token, sep);
        for(char c : token){
            set_castling(c, true);
        }

        // en passant square
        getline(ss, token, sep);
        // TODO en passant

        // nb reversible plies
        getline(ss, token, sep);
        reversible_plies = std::stoi(token);

        // nb moves
        getline(ss, token, sep);
        int moves = std::stoi(token);
        plies = 2 *  moves + turn - 1;
    }

    Color Position::get_turn(){
        return plies % 2 ? WHITE : BLACK;
    }

    void Position::set_turn(Color color){
        // turn is set from nb of moves
    }

    void Position::set_castling(Piece cast_type, bool right){
        U64 cast_mask = 0;
        switch(cast_type){
            case Board::WHITE_KING:
                cast_mask = WHITE_KINGSIDE_CAST;
                break;
            case Board::WHITE_QUEEN:
                cast_mask = WHITE_QUEENSIDE_CAST;
                break;
            case Board::BLACK_KING:
                cast_mask = BLACK_KINGSIDE_CAST;
                break;
            case Board::BLACK_QUEEN:
                cast_mask = BLACK_QUEENSIDE_CAST;
                break;
            default:
                return;
        }
        if(right){
            castling |= cast_mask;
        } else {
            castling &= ~cast_mask;
        }
    }

    bool Position::get_castling(Piece cast_type){
        U64 cast_mask = 0;
        switch(cast_type){
            case Board::WHITE_KING:
                cast_mask = WHITE_KINGSIDE_CAST;
                break;
            case Board::WHITE_QUEEN:
                cast_mask = WHITE_QUEENSIDE_CAST;
                break;
            case Board::BLACK_KING:
                cast_mask = BLACK_KINGSIDE_CAST;
                break;
            case Board::BLACK_QUEEN:
                cast_mask = BLACK_QUEENSIDE_CAST;
                break;
            default:
                break; // TODO : generate Exception ?
        }
        return bool(castling & cast_mask);
    }

    std::string Position::get_all_castling(){
        std::string castling = "";
        if(get_castling(Board::WHITE_KING)) {
            castling += Board::WHITE_KING;
        }
        if(get_castling(Board::WHITE_QUEEN)) {
            castling += Board::WHITE_QUEEN;
        }
        if(get_castling(Board::BLACK_KING)) {
            castling += Board::BLACK_KING;
        }
        if(get_castling(Board::BLACK_QUEEN)) {
            castling += Board::BLACK_QUEEN;
        }
        if(castling == ""){
            castling = "-";
        }
        return castling;
    }

    std::string Position::get_en_passant(){
        return square_to_coordinate(en_passant);
    }

    void Position::reset(){
        board.empty();
        castling = 0;
        en_passant = 0;
        plies = 1;
        reversible_plies = 0;
    }
}
