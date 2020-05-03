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
        board[get_layer(piece)] |= bitmask;
    }

    Piece Board::get_square(int square){
        U64 bitmask = get_bitmask(square);
        for(int l = 0; l < NB_LAYERS; l++){
            if(board[l] & bitmask){
                return get_piece(l);
            }
        }
        return EMPTY;
    }

    void Board::empty(){
        for(int layer = 0; layer < NB_LAYERS; layer++){
            board[layer] = 0;
        }
    }

    Piece Board::get_piece(int layer){
        return PIECES[layer];
    }

    int Board::get_layer(Piece piece){
        switch(piece){
            case EMPTY:
                return INVALID_LAYER;
            case WHITE_PAWN:
                return WHITE_PAWN_LAYER;
            case WHITE_KNIGHT:
                return WHITE_KNIGHT_LAYER;
            case WHITE_BISHOP:
                return WHITE_BISHOP_LAYER;
            case WHITE_ROOK:
                return WHITE_ROOK_LAYER;
            case WHITE_QUEEN:
                return WHITE_QUEEN_LAYER;
            case WHITE_KING:
                return WHITE_KING_LAYER;
            case BLACK_PAWN:
                return BLACK_PAWN_LAYER;
            case BLACK_KNIGHT:
                return BLACK_KNIGHT_LAYER;
            case BLACK_BISHOP:
                return BLACK_BISHOP_LAYER;
            case BLACK_ROOK:
                return BLACK_ROOK_LAYER;
            case BLACK_QUEEN:
                return BLACK_QUEEN_LAYER;
            case BLACK_KING:
                return BLACK_KING_LAYER;
            default:
                return INVALID_LAYER;
        }
    }

    int Board::coordinates_to_square(std::string coordinates){
        char file = coordinates[0];
        char rank = coordinates[1];
        if (file < 'a' || file > 'h' || rank < '1' || rank > '8'){
            return -1; // TODO throw Exception ?
        }
        return file - 'a' + (rank - '1') * 8;
    }

    std::string Board::square_to_coordinate(int square){
        std::string file(1, char('a' + square % 8));
        std::string rank(1, char('1' + square / 8));
        return file + rank;
    }

    void Board::make_move(Move* move){
        // U8 from_layer = move->get_from_layer();
        U8 from_square = move->get_from_square();
        U8 to_layer = move->get_to_layer();
        U8 to_square = move->get_to_square();
        // U8 take = move->get_take_square();
        U64 from_mask = U64(1) << from_square;
        U64 to_mask = U64(1) << to_square;
        U64 take_mask = 0; //U64(1) << take;
        U64 free_squares = ~0;
        for(int l = 0; l < NB_LAYERS; l++){
            free_squares &= ~board[l];
        }
        if((to_layer == WHITE_PAWN_LAYER) && (to_mask & free_squares)){
            take_mask = to_mask >> 8; // en passant
        }
        if((to_layer == BLACK_PAWN_LAYER) && (to_mask & free_squares)){
            take_mask = to_mask << 8; // en passant
        }
        for(int l = 0; l < NB_LAYERS; l++){
            board[l] &= ~(from_mask | to_mask | take_mask);
        }
        if(to_layer == WHITE_KING_LAYER){
            if(from_square == 4 && to_square == 6){
                // white kingside castling
                board[WHITE_ROOK_LAYER] |= U64(1) << 5;
                board[WHITE_ROOK_LAYER] &= ~(U64(1) << 7);
            } else if(from_square == 4 && to_square == 2){
                // white queenside castling
                board[WHITE_ROOK_LAYER] |= U64(1) << 3;
                board[WHITE_ROOK_LAYER] &= ~(U64(1) << 0);
            }
        } else if(to_layer == BLACK_KING_LAYER){
            if(from_square == 60 && to_square == 62){
                // black kingside castling
                board[BLACK_ROOK_LAYER] |= U64(1) << 61;
                board[BLACK_ROOK_LAYER] &= ~(U64(1) << 63);
            } else if(from_square == 60 && to_square == 58){
                // black queenside castling
                board[BLACK_ROOK_LAYER] |= U64(1) << 59;
                board[BLACK_ROOK_LAYER] &= ~(U64(1) << 56);
            }
        }
        board[to_layer] |= to_mask;
    }

    void Board::print(){
        for(int l = 0; l < NB_LAYERS; l++){
            std::bitset<64> bs(board[l]);
            std::cout << bs << std::endl;
        }
    }

    Position::Position() : board(){
        reset();
    }

    void Position::set_start_position(){
        import_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }

    std::string Position::export_fen(){
        std::string fen;
        int nb_empty = 0;
        for(int i = 0; i < 64; i++){
            int file = i % 8;
            int rank = i / 8;
            Piece piece = board.get_square((7 - rank) * 8 + file);
            if(piece == Board::EMPTY){
                nb_empty++;
            } else {
                if(nb_empty > 0){
                    fen += std::to_string(nb_empty);
                    nb_empty = 0;
                }
                fen += piece;
            }
            if(i % 8 == 7 && i != 63){
                if(nb_empty > 0){
                    fen += std::to_string(nb_empty);
                    nb_empty = 0;
                }
                fen += "/";
            }
        }
        return fen + " " + std::string(1, get_turn())
            + " " + get_all_castling()
            + " " + get_en_passant()
            + " " + std::to_string(get_reversible_plies())
            + " " + std::to_string(get_move());
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
        set_en_passant(token);

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

    void Position::set_en_passant(std::string ep){
        int square = Board::coordinates_to_square(ep);
        if(square <= 0) {
            en_passant = 0;
            return;
        }
        en_passant = U8(1) << (square % 8);
    }

    std::string Position::get_en_passant(){
        if(!en_passant){
            return "-";
        }
        return std::string(1, 'a' + __builtin_ffsll(en_passant) - 1)
               + (get_turn() == WHITE ? "6" : "3");
    }

    int Position::get_move(){
        return (plies + 1) / 2;
    }

    int Position::get_plies(){
        return plies;
    }

    int Position::get_reversible_plies(){
        return reversible_plies;
    }

    std::string Position::get_square(int square){
        return std::string(1, board.get_square(square));
    }

    void Position::reset(){
        board.empty();
        castling = 0;
        en_passant = 0;
        plies = 1;
        reversible_plies = 0;
    }

    void Position::make_move(Move* move){
        plies++;
        board.make_move(move);
        en_passant = 0;

        // set rerversible moves
        if(move->get_from_layer() == Board::WHITE_PAWN_LAYER
            || move->get_from_layer() == Board::BLACK_PAWN_LAYER){
            reversible_plies = 0;
        } else{
            // TODO : check attacks (or number of pieces);
            reversible_plies++;
        }

        // set en passant
        if(move->get_from_layer() == Board::WHITE_PAWN_LAYER
            && (move->get_to_square() - move->get_from_square() == 16)){
            en_passant =  U8(1) << ((move->get_to_square() - 8) % 8);
        }
        if(move->get_from_layer() == Board::BLACK_PAWN_LAYER
            && (move->get_to_square() - move->get_from_square() == -16)){
            en_passant =  U8(1) << ((move->get_to_square() + 8) % 8);
        }

        // set castling rights
        if(move->get_to_layer() == Board::WHITE_KING_LAYER){
            set_castling(Board::WHITE_KING, false);
            set_castling(Board::WHITE_QUEEN, false);
        } else if(move->get_to_layer() == Board::BLACK_KING_LAYER){
            set_castling(Board::BLACK_KING, false);
            set_castling(Board::BLACK_QUEEN, false);
        }
        if(move->get_to_layer() == Board::WHITE_ROOK_LAYER){
            if(move->get_from_square() == 7){
                set_castling(Board::WHITE_KING, false);
            } else if(move->get_from_square() == 0){
                set_castling(Board::WHITE_QUEEN, false);
            }
        } else if(move->get_to_layer() == Board::BLACK_ROOK_LAYER){
            if(move->get_from_square() == 63){
                set_castling(Board::BLACK_KING, false);
            } else if(move->get_from_square() == 56){
                set_castling(Board::BLACK_QUEEN, false);
            }
        }
    }

    Move Position::get_move_from_long_algebraic(const std::string &m){
        int from = Board::coordinates_to_square(m.substr(0, 2));
        int to = Board::coordinates_to_square(m.substr(2, 4));
        int layer = Board::INVALID_LAYER;
        if(m.length() == 5){
            char promotion = m.substr(4, 5)[0];
            switch(promotion){
                case 'n':
                    layer = Board::WHITE_KNIGHT_LAYER;
                    break;
                case 'b':
                    layer = Board::WHITE_BISHOP_LAYER;
                    break;
                case 'r':
                    layer = Board::WHITE_ROOK_LAYER;
                    break;
                case 'q':
                    layer = Board::WHITE_QUEEN_LAYER;
                    break;
            }
        }
        MoveGenerator generator(this);
        generator.generate();
        for(Move m : generator.moveList){
            if(m.get_from_square() == from && m.get_to_square() == to){
                if(layer != Board::INVALID_LAYER){
                    if(m.get_to_layer() != layer && m.get_to_layer() != layer + Board::BLACK_PAWN_LAYER){
                        continue;
                    }
                }
                return m;
            }
        }
        return Move();
    }
}
