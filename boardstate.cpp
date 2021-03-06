#include "boardstate.h"
#include "evaluation.h"

namespace BoardTools{
    
    int apply(Board* state, int playerId, Move move){
        //raise movecount
        ++state->movecount;
//        if(move.to >= 60){
//            return state->movecount;
//        }
        //mark used fieldas used
        state->used |= 1ULL << move.to;
//        state->used &= ~FIT;
        if(playerId == ID_WE){
            //mark new pos 
            state->mypos |= 1ULL << move.to;
            //unmark old pos
            state->mypos &= ~(1ULL << move.from);
            //add points of new field
            state->pointsdiff += ((Globals::threes >> move.to) & 1ULL) * 3 + ((Globals::twos >> move.to) & 1ULL) * 2 + ((Globals::ones >> move.to) & 1ULL);
        }else{
            state->oppos |= 1ULL << move.to;
            state->oppos &= ~(1ULL << move.from);
            state->pointsdiff -= ((Globals::threes >> move.to) & 1ULL) * 3 + ((Globals::twos >> move.to) & 1ULL) * 2 + ((Globals::ones >> move.to) & 1ULL);
        }
        return state->movecount;
    }
    
    int unapply(Board* state, int playerId, Move move){
        --state->movecount;
//        if(move.to >= 60){
//            return state->movecount;
//        }
        state->used &= ~(1ULL << move.to);
//        state->used &= ~FIT;
        if(playerId == ID_WE){
            state->mypos &= ~(1ULL << move.to);
            state->mypos |= 1ULL << move.from;
            state->pointsdiff -= ((Globals::threes >> move.to) & 1ULL) * 3 + ((Globals::twos >> move.to) & 1ULL) * 2 + ((Globals::ones >> move.to) & 1ULL);
        }else{
            state->oppos &= ~(1ULL << move.to);
            state->oppos |= 1ULL << move.from;
            state->pointsdiff += ((Globals::threes >> move.to) & 1ULL) * 3 + ((Globals::twos >> move.to) & 1ULL) * 2 + ((Globals::ones >> move.to) & 1ULL);
        }
        return state->movecount;
    }
    
    
    bool isValidMove(Board state, Move move, int player){
        return 
                //move must be valid
                (move.from <= INVALID_POS && move.to <= INVALID_POS) &&
                //if mc < 8                is setmove                                                 field is free
                (((state.movecount < 8) && (move.from == INVALID_POS) && (move.to != INVALID_POS) && !(state.used >> move.to & 1ULL)) 
                //else if mc >= 8               is no setmove
                || ((state.movecount >= 8) && !(move.from == INVALID_POS && move.to != INVALID_POS) && 
                //is nullmove
                (((move.from == INVALID_POS) && (move.to == INVALID_POS))
                //or is runmove to free field             if our turn          and own field
                || (!(state.used >> move.to & 1ULL) && (((player == ID_WE) && (state.mypos >> move.from & 1ULL))
                //           or if opponents turn and its field
                        || ((player != ID_WE) && (state.oppos >> move.from & 1ULL)))))));
    }
    
    /**
     * 
     * @param valuedMoves the pointer to the first element of the move array in which the move should be insert
     * @param m the move
     * @param state
     * @param playerId 
     * @param length of move array
     * @return value of move
     */
    int insertMove(Move* valuedMoves, Move m, Board state, int playerId, int length){
        //copy state
        Board tmpBoard = state;
        //apply board
        BoardTools::apply(&tmpBoard, playerId, m);
        //evaluate result inaccurate
        int value = Evaluation::fastEvaluate(playerId, tmpBoard);  
        m.value = value;
        //valuedMoves[i] = vm;
        int b = length-1;
        
        //untested
        if(b > MOVE_GEN_LIMIT){
            b = MOVE_GEN_LIMIT;
        }
        //insert move based on value to sorted movelist
        for(; b > 0; --b){
            if(valuedMoves[b-1].value < value){
                valuedMoves[b] = valuedMoves[b-1];
            }else{
                valuedMoves[b] = m;
                break;
            }     
        }
        if(!b){
            valuedMoves[0] = m;
        }
        return value;
    }
    
    /**
    //This method returns all valid moves for the player. 
    //The moves should be sorted best to bad. Consequently the NullMove is the lastMove
     * 
     * @param playerId
     * @param length the length of the resulting array
     * @return an array of all valid moves for the player preferable sorted form best to bad
     */
    Move* generatePossibleMoves(Board state, int playerId, int *length){
        //if not in set mode return null move
        //std::cout << "Gmc " << getMoveCount() << std::endl;
        
        if(state.movecount >= 8){
            //if in move-phase
            
            //std::cout << "PlayerId " << playerId << std::endl;
            u_int64_t penguinPositions = ((playerId == ID_WE) ? state.mypos : state.oppos);
            //BitBoard::printField(penguinPositions);
            
            //fetch penguin positions in an int-list
            int l = 0;
            int* penguinPos = Tools::fastBitScan(penguinPositions, &l);

            //generate move fields for every penguin
            u_int64_t *moveFields = new u_int64_t[4];

            moveFields[0] = Tools::genMoveField(penguinPos[0], state.used);
            moveFields[1] = Tools::genMoveField(penguinPos[1], state.used);
            moveFields[2] = Tools::genMoveField(penguinPos[2], state.used);
            moveFields[3] = Tools::genMoveField(penguinPos[3], state.used);

            //count the number of possible moves
            *length = Tools::popCount(moveFields[0])
                + Tools::popCount(moveFields[1])
                + Tools::popCount(moveFields[2])
                + Tools::popCount(moveFields[3]);
            
            //add null move if no other move is possible
            bool nullMove = false;
            if(*length <= 0){
                *length = 1;
                nullMove = true;
            }
            
            //create aaray of moves
            Move* moves = new Move[*length];
            //std::cout << "Length: " << *length << std::endl;

            int c = -1;
            for(int i = 0; i < 4; i++){
                //sort moves by field type
                u_int64_t threes = Globals::threes & moveFields[i];
                u_int64_t twos = Globals::twos & moveFields[i];
                u_int64_t ones = Globals::ones & moveFields[i];

                //std::cout << "penguin" << i << " - " << penguinPos[i] << std::endl;

                //add moves which are targeting fields with 3 fishes first
                int threesSize = 0;
                //get the number of moves
                int* threesPos = Tools::bitScan(threes, &threesSize);
                //create each move
                for(int k = 0; k < threesSize; k++){
                    Move m = Move();
                    m.from = penguinPos[i];
                    m.to = threesPos[k];
                    //add each move to the move-list
                    insertMove(moves, m, state, playerId, *length);
                }
                delete[] threesPos;

                int twosSize = 0;
                int* twosPos = Tools::bitScan(twos, &twosSize);
                for(int k = 0; k < twosSize; k++){
                    Move m = Move();
                    m.from = penguinPos[i];
                    m.to = twosPos[k];
                    insertMove(moves, m, state, playerId, *length);
                }
                delete[] twosPos;

                int onesSize = 0;
                int* onesPos = Tools::bitScan(ones, &onesSize);
                for(int k = 0; k < onesSize; k++){
                    Move m = Move();
                    m.from = penguinPos[i];
                    m.to = onesPos[k];
                    insertMove(moves, m, state, playerId, *length);
                }
                delete[] onesPos;
                
            }
            
            if(nullMove){
                //add null move to list
                Move m = Move();
                m.from = INVALID_POS;
                m.to = INVALID_POS;
                insertMove(moves, m, state, playerId, *length);
            }
            
            delete[] moveFields, penguinPos;
            //cut list to limit
            if(*length > MOVE_GEN_LIMIT){
                *length = MOVE_GEN_LIMIT;
            }
            return moves;
        }        
        //else return setMove
        //fetch free fields with one fish
        u_int64_t freeSetPositions = Globals::ones & ~state.used;
        int freePosSize = 0;
        //get number of free positions
        int* freePos = Tools::bitScan(freeSetPositions,  &freePosSize);
        //std::cout << "FreePosSize " << freePosSize << std::endl;
        *length = freePosSize;
        //create list for set-moves
        Move* moves = new Move[freePosSize];

        //add every set-move to list
        for(int i = 0; i < freePosSize; i++){
            #ifdef DEBUG_MOVEGEN
                    std::cout << "moveGen: Adding move: From: " << " To: " << freePos[i] << std::endl; 
            #endif
            Move m = Move();
            m.from = INVALID_POS;
            m.to = freePos[i];
            insertMove(moves, m, state, playerId, freePosSize);
        }
        
        //cut list to limit
        if(*length > MOVE_GEN_LIMIT){
            *length = MOVE_GEN_LIMIT;
        }
        delete[] freePos;
        return moves; 
    }
    
    
    Move generateGoodMove(Board state, int playerId){
        if(state.movecount >= 8){
            //std::cout << "PlayerId " << playerId << std::endl;
            u_int64_t penguinPositions = ((playerId == ID_WE) ? state.mypos : state.oppos);
            //BitBoard::printField(penguinPositions);

            //find position of one penguin
            int from = Tools::bitScanForward(penguinPositions);
            //generate moveField for that penguin
            u_int64_t moveField = Tools::genMoveField(from, state.used);
            //try to use the most valuable move
            int to;
            if(Globals::threes & moveField){
                //find first move to a field with three fishes
                to = Tools::bitScanForward(Globals::threes & moveField);
            }else if(Globals::twos & moveField){
                to = Tools::bitScanForward(Globals::twos & moveField);
            }else if(Globals::ones & moveField){
                to = Tools::bitScanForward(Globals::ones & moveField);
            }else{
                //null move mustn't be the only move but we don'T want to look 
                //at evry penguin. and when one penguin is catched it is probably 
                //that the game ends son and this search isn't important anymore.
                to = INVALID_POS;
                from = INVALID_POS;
            }
            Move result = Move();
            result.from = from;
            result.to = to;
            return result;
        }
        //return a set move
        Move result = Move();
        result.from = INVALID_POS;
        result.to = Tools::bitScanForward(Globals::ones & ~state.used);
        return result;
    }
};
