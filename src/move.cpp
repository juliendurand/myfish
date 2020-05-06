#include <bitset>
#include <iostream>

#include "move.h"

namespace chess {

    void printBitset(std::string s, U64 bits){
        std::cout << s << ": " << std::endl;
        for(int i = 0; i < 8; i++){
            U64 rank = (bits >> ((7-i) * 8)) & 255;
            U64 swap = 0;
            for(int j = 0; j < 8; j++){
                swap |= (((rank & (U64(1) << j)) > 0) << (7 - j));
            }
            std::cout << std::bitset<8>(swap) << std::endl;
        }
    }

    U64 perft(int depth, Position* position, bool printinfo){
        U64 nodes = 0;
        U64 n_nodes = 0;
        MoveGenerator generator(position);
        n_nodes = U64(generator.generate());
        if(depth == 1 && !printinfo){
            return n_nodes;
        }
        for(Move m : generator.moveList){
            if(depth > 1){
                Position new_position(*position);
                new_position.make_move(&m);
                n_nodes = perft(depth - 1, &new_position, false);
            }else{
                n_nodes = 1;
            }
            nodes += n_nodes;
            if(printinfo){
                std::cout << m.to_long_algebraic() << ": " << n_nodes << std::endl;
            }
        }
        return nodes;
    }

    /*
    * Move
    */

    void Move::set(U8 from_layer, U8 from_square, U8 to_layer, U8 to_square){
        this->from_layer = from_layer;
        this->from_square = from_square;
        this->to_layer = to_layer;
        this->to_square = to_square;
    }

    U8 Move::get_from_layer(){
        return from_layer;
    }

    Piece Move::get_from_piece(){
        return Board::get_piece(get_from_layer());
    }

    U8 Move::get_from_square(){
        return from_square;
    }

    U8 Move::get_to_layer(){
        return to_layer;
    }

    Piece Move::get_to_piece(){
        return Board::get_piece(get_to_layer());
    }

    U8 Move::get_to_square(){
        return to_square;
    }

    bool Move::is_promotion(){
        return from_layer != to_layer;
    }

    std::string Move::to_long_algebraic(){
        std::string s = Board::square_to_coordinate(get_from_square())
                      + Board::square_to_coordinate(get_to_square());
        if(is_promotion()){
            s.push_back(std::tolower(Board::get_piece(get_to_layer())));
        }
        return s;
    }

    /*
    * Move Generator
    */

    MoveGenerator::MoveGenerator(Position* pos){
        position = pos;
        turn = position->get_turn() == Position::WHITE ? 0 : Board::NB_LAYERS / 2;
        opponent = (turn + Board::NB_LAYERS / 2) % Board::NB_LAYERS;
        layer = 0;
        square_mask = ~U64(0);
        seq = 0;
        own_pieces = position->board.board[turn]
            | position->board.board[turn + 1]
            | position->board.board[turn + 2]
            | position->board.board[turn + 3]
            | position->board.board[turn + 4]
            | position->board.board[turn + 5];
        opponent_pieces = position->board.board[opponent]
            | position->board.board[opponent + 1]
            | position->board.board[opponent + 2]
            | position->board.board[opponent + 3]
            | position->board.board[opponent + 4]
            | position->board.board[opponent + 5];
        all_pieces = own_pieces | opponent_pieces;
        free_square = ~all_pieces;
    }

    int MoveGenerator::generate(){
        generate_all();
        for(auto m : allList){
            if(!ischeck(&m)){
               moveList.push_back(m);
            }
        }
        return moveList.size();
    }

    int MoveGenerator::generate_all(){
        U64 notOwnPieces = ~own_pieces;
        int layer;
        U64 pieces;
        U64 movebits;
        U64 en_passant_attacks = U64(position->en_passant) << ((position->get_turn() == Position::WHITE) ? 40 : 16);
        layer = turn + Board::WHITE_PAWN_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_pawn_pushes(p, position->get_turn(), free_square);
            generate_move_bitscan(layer, from, layer, movebits);
            movebits = generate_pawn_push_promotions(p, position->get_turn(), free_square);
            if(movebits){
                generate_move_bitscan(layer, from, layer + Board::WHITE_KNIGHT_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_BISHOP_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_ROOK_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_QUEEN_LAYER, movebits);
            }
            movebits = generate_pawn_double_pushes(p, position->get_turn(), free_square);
            generate_move_bitscan(layer, from, layer, movebits);
            movebits = generate_pawn_attacks(p, position->get_turn(), notOwnPieces) & (opponent_pieces | en_passant_attacks);
            generate_move_bitscan(layer, from, layer, movebits);
            movebits = generate_pawn_promotion_attacks(p, position->get_turn(), notOwnPieces) & opponent_pieces;
            if(movebits){
                generate_move_bitscan(layer, from, layer + Board::WHITE_KNIGHT_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_BISHOP_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_ROOK_LAYER, movebits);
                generate_move_bitscan(layer, from, layer + Board::WHITE_QUEEN_LAYER, movebits);
            }
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_KNIGHT_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_knight_attacks(p, notOwnPieces);
            generate_move_bitscan(layer, from, layer, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_BISHOP_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_bishop_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, layer, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_ROOK_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_rook_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, layer, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_QUEEN_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_queen_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, layer, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_KING_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_king_attacks(p, notOwnPieces);
            generate_move_bitscan(layer, from, layer, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        generate_castling();

        return allList.size();
    }

    void MoveGenerator::generate_move_bitscan(int from_layer, int from, int to_layer, U64 bits){
        if(bits) do {
            int idx = __builtin_ffsll(bits) - 1;
            Move m;
            m.set(from_layer, from, to_layer, idx);
            allList.push_back(m);
        } while (bits &= bits - 1); // reset LS1B
    }

    bool MoveGenerator::ischeck(Move* m){
        int from_square = m->get_from_square();
        U64 from = U64(1) << from_square;
        int from_layer = m->get_from_layer();
        int to_square = m->get_to_square();
        U64 to = U64(1) << to_square;

        // en passant
        U64 ep = U64(0);
        if((from_layer - turn == 0)
            && (((from_square - to_square) % 8) != 0) // not on the same file
            && (to & ~opponent_pieces)){
                ep = (from_layer == Board::WHITE_PAWN_LAYER) ? to >> 8 : to << 8;
        }
        U64 notOpponentPieces = ~opponent_pieces | to | ep;
        U64 notOwnPieces = (~own_pieces | from) & ~to;
        U64 free_square = notOwnPieces & notOpponentPieces;
        U64 attacks = generate_attacks(to | ep, notOpponentPieces, free_square);

        int king_layer = turn + Board::WHITE_KING_LAYER;
        U64 ourKing = position->board.board[king_layer] & ~from;
        ourKing = ourKing | (!ourKing  * to);
        return attacks & ourKing;
    }

    U64 MoveGenerator::generate_attacks(U64 to, U64 notOpponentPieces, U64 free_square){
        int w = position->get_turn() == Position::WHITE ? Position::BLACK : Position::WHITE;
        U64 attacks = generate_pawn_attacks(position->board.board[opponent + Board::WHITE_PAWN_LAYER] & ~to, w, notOpponentPieces);
        attacks |= generate_pawn_promotion_attacks(position->board.board[opponent + Board::WHITE_PAWN_LAYER] & ~to, w, notOpponentPieces);
        attacks |= generate_knight_attacks(position->board.board[opponent + Board::WHITE_KNIGHT_LAYER] & ~to, notOpponentPieces);
        attacks |= generate_bishop_attacks((position->board.board[opponent + Board::WHITE_BISHOP_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER]) & ~to, notOpponentPieces, free_square);
        attacks |= generate_rook_attacks((position->board.board[opponent + Board::WHITE_ROOK_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER]) & ~to, notOpponentPieces, free_square);
        attacks |= generate_king_attacks(position->board.board[opponent + Board::WHITE_KING_LAYER] & ~to, notOpponentPieces);
        return attacks;
    }

    U64 aFileMask = 0xFEFEFEFEFEFEFEFE;
    U64 hFileMask = 0x7F7F7F7F7F7F7F7F;
    U64 abFileMask = 0xFCFCFCFCFCFCFCFC;
    U64 ghFileMask = 0x3F3F3F3F3F3F3F3F;

    U64 MoveGenerator::generate_pawn_pushes(U64 layer, Color color, U64 free_square){
        int w = color == Position::WHITE;
        int b = 1 - w;
        U64 pushes = layer << (8 * w) | layer >> (8 * b);
        pushes &= 0x00FFFFFFFFFF0000 * w + 0x0000FFFFFFFFFF00 * b;
        return pushes & free_square;
    }

    U64 MoveGenerator::generate_pawn_push_promotions(U64 layer, Color color, U64 free_square){
        int w = color == Position::WHITE;
        int b = 1 - w;
        U64 pushes = layer << (8 * w) | layer >> (8 * b);
        pushes &= 0xFF00000000000000 * w + 0x00000000000000FF * b;
        return pushes & free_square;
    }

    U64 MoveGenerator::generate_pawn_double_pushes(U64 layer, Color color, U64 free_square){
        U64 pushes = 0;
        if(color == Position::WHITE){
            pushes = (((layer & 0x000000000000FF00) << 8) & free_square) << 8;
        }else{
            pushes = (((layer & 0x00FF000000000000) >> 8) & free_square) >> 8;
        }
        return pushes & free_square;
    }

    U64 MoveGenerator::generate_pawn_attacks(U64 layer, Color color, U64 notSelf){
        U64 attacks = 0;
        if(color == Position::WHITE){
            attacks = (layer & aFileMask) << 7
                    | (layer & hFileMask) << 9;
            attacks &= 0x00FFFFFFFFFF0000;
        }else{
            attacks = (layer & hFileMask) >> 7
                    | (layer & aFileMask) >> 9;
            attacks &= 0x0000FFFFFFFFFF00;
        }
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_pawn_promotion_attacks(U64 layer, Color color, U64 notSelf){
        U64 attacks = 0;
        if(color == Position::WHITE){
            attacks = (layer & aFileMask) << 7
                    | (layer & hFileMask) << 9;
            attacks &= 0xFF00000000000000;
        }else{
            attacks = (layer & hFileMask) >> 7
                    | (layer & aFileMask) >> 9;
            attacks &= 0x00000000000000FF;
        }
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_knight_attacks(U64 layer, U64 notSelf){
        U64 attacks = (layer & hFileMask) << 17
                     | (layer & aFileMask) << 15
                     | (layer & ghFileMask) << 10
                     | (layer & abFileMask) << 6
                     | (layer & ghFileMask) >> 6
                     | (layer & abFileMask) >> 10
                     | (layer & hFileMask) >> 15
                     | (layer & aFileMask) >> 17;
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_bishop_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = expandNE(layer, notSelf, free_square)
                    | expandSE(layer, notSelf, free_square)
                    | expandSW(layer, notSelf, free_square)
                    | expandNW(layer, notSelf, free_square);
        return attacks;
    }

    U64 MoveGenerator::generate_rook_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = expandN(layer, notSelf, free_square)
                    | expandE(layer, notSelf, free_square)
                    | expandS(layer, notSelf, free_square)
                    | expandW(layer, notSelf, free_square);
        return attacks;
    }

    U64 MoveGenerator::generate_queen_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = generate_bishop_attacks(layer, notSelf, free_square)
                    | generate_rook_attacks(layer, notSelf, free_square);
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_king_attacks(U64 layer, U64 notSelf){
        U64 attacks = (layer & aFileMask) << 7
             | (layer & aFileMask) >> 9
             | (layer & hFileMask) << 9
             | (layer & hFileMask) >> 7
             | layer << 8
             | layer >> 8
             | (layer & hFileMask) << 1
             | (layer & aFileMask) >> 1;
        return attacks & notSelf;
    }

    void MoveGenerator::generate_castling(){
        Move m;
        int layer = Board::WHITE_KING_LAYER + turn;
        int b = turn != 0;
        int from = 4 + 56 * b;
        int to_kingside = 6 + 56 * b;
        int to_queenside = 2 + 56 * b;
        U64 kingside_free_square = U64(0x60) << (56 * b);
        U64 queenside_free_square = U64(0xE) << (56 * b);
        U64 kingside_castling_squares = U64(0x70) << (56 * b);
        U64 queenside_castling_squares = U64(0x1C) << (56 * b);

        U64 free_square = ~own_pieces & ~opponent_pieces;
        U64 attacks = generate_attacks(U64(0), ~opponent_pieces, free_square);
        if(position->get_castling(b ? Board::BLACK_KING : Board::WHITE_KING) 
            && !(attacks & kingside_castling_squares) 
            && ((free_square & kingside_free_square) == kingside_free_square)){
                m.set(layer, from, layer, to_kingside);
                moveList.push_back(m);
        }
        if(position->get_castling(b ? Board::BLACK_QUEEN : Board::WHITE_QUEEN)
            && !(attacks & queenside_castling_squares) 
            && ((free_square & queenside_free_square) == queenside_free_square)){
                m.set(layer, from, layer, to_queenside);
                moveList.push_back(m);
        }
    }

    U64 MoveGenerator::expandN(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = layer << 8;
        attacks |= (attacks & free_square) << 8;
        attacks |= (attacks & free_square) << 8;
        attacks |= (attacks & free_square) << 8;
        attacks |= (attacks & free_square) << 8;
        attacks |= (attacks & free_square) << 8;
        attacks |= (attacks & free_square) << 8;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandNE(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        attacks |= (attacks & free_square & hFileMask) << 9;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandE(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        attacks |= (attacks & free_square & hFileMask) << 1;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandSE(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        attacks |= (attacks & free_square & hFileMask) >> 7;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandS(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = layer >> 8;
        attacks |= (attacks & free_square) >> 8;
        attacks |= (attacks & free_square) >> 8;
        attacks |= (attacks & free_square) >> 8;
        attacks |= (attacks & free_square) >> 8;
        attacks |= (attacks & free_square) >> 8;
        attacks |= (attacks & free_square) >> 8;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandSW(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        attacks |= (attacks & free_square & aFileMask) >> 9;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandW(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        attacks |= (attacks & free_square & aFileMask) >> 1;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandNW(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        attacks |= (attacks & free_square & aFileMask) << 7;
        return attacks & notSelf;
    }
}
