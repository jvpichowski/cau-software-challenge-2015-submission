#include "main.h"
#include "tactic.h"
#include <sstream>  // Required for stringstreams
#include <string> 


bool isInitialized = false;
bool sentMove = false;


void onPacket(char* msg){
    #ifdef DEBUG_OCEAN
        std::cout << msg << std::endl;
    #endif
    Fisher::Catch(msg);
}

void onTick(){
}


void onFieldRec(u_int64_t ones, u_int64_t twos, u_int64_t threes)
{
    #ifdef DEBUG_CALLS
        cout << "DEBUG_CALLS: onFieldRec called" << endl;
    #endif
    
    if(isInitialized){
        #ifdef DEBUG_CALLS
            cout << "DEBUG_CALLS: onFieldRec canceled" << endl;
        #endif
        return;
    }
        
    Globals::ones = ones;
    Globals::twos = twos;
    Globals::threes = threes;
    
    Globals::_board = Board();
    
    isInitialized = true;
    
    //create Board
    #ifdef DEBUG_MAIN_SETFIELD
        Tools::printField(ones ^ twos ^ threes);
    #endif
}


/**
 * On getting moveRequestion
 */
void onMoveReq()
{
    
    #ifdef DEBUG_CALLS
        cout << "DEBUG_CALLS: onMoveReq called" << endl;
    #endif

    // pass the starting time to the calculate function and let the calculate function decide when to return.
    // We do not use multiple threads and therefore we can't have a coroutine checking the time.
    //Globals::moveReqTime = clock();
    clock_gettime(Globals::clockTime, &Globals::moveReqTime); 
    
    /////// TODO: MoveCalculation in here!
    
    //BitBoard::printBoard(_board);
    
    Move move;
    //there is a bug when the game ends no move is send
    //cout << "MTDf: " << Navi::alphaBetaTT(_board, 3, MIN_AB_VALUE, MAX_AB_VALUE,ID_WE, &move) << endl;
    //cout << "minimax: " << Navi::miniMax(ID_WE, 5, MIN_AB_VALUE, MAX_AB_VALUE,_board, &move) << endl;
//    std::cout << "Board pre calc: " << std::endl;
//    std::cout << "Used:" << std::endl;
//    Tools::printField(_board->used);
//    std::cout << "Mypos:" << std::endl;
//    Tools::printField(_board->mypos);
//    std::cout << "Oppos:" << std::endl;
//    Tools::printField(_board->oppos);
//    std::cout << "movecount: " << _board->movecount << " pointsdiff: " << _board->pointsdiff << std::endl;
  
    
//    u_int64_t activePlayer;
//    int rlng;
//    Move preResult;
//    
//    int preEvalRes = Tactic::preliminaries(&preResult, &rlng, &activePlayer);
//    
//    if(preEvalRes == 0)
//    {
        Evaluation::preEvaluate();
        
//        std::cout << "used: " << Globals::_board.used << std::endl;
//        Tools::printField(Globals::_board.used);
//        std::cout << "mypos: " << Globals::_board.mypos << std::endl;
//        Tools::printField(Globals::_board.mypos);
//        std::cout << "oppos: " << Globals::_board.oppos << std::endl;
//        Tools::printField(Globals::_board.oppos);
//        std::cout << "threes: " << Globals::threes << std::endl;
//        Tools::printField(Globals::threes);
//        std::cout << "twos: " << Globals::threes << std::endl;
//        Tools::printField(Globals::threes);
//        std::cout << "ones: " << Globals::threes << std::endl;
//        Tools::printField(Globals::threes);
//        
//        exit(0);
        //move = startPSearch(15, ID_WE, Globals::_board);
        //std::cout << "PSearchTo: " << (u_int64_t)startPSearch(5, ID_WE, Globals::_board).to << std::endl;
    
        std::cout << "IT: " << iterativeDeepening(Globals::_board, ID_WE, 60, 0, &move) << std::endl;//change first guess to 100?
        std::cout << "MT: " << (u_int64_t)move.from << "->" << (u_int64_t)move.to << std::endl;
        std::cout << "Used before: " << Globals::_board.used << std::endl;
//    }
//    else
//    {
//        move = preResult;
//        printf("\n\npppppppppppppppppppppppppppppppprevalUsed\n\n");
//    }
        
        
//    if(move.from == move.to && move.from != INVALID_POS){
//        std::cout << "======================================================" << std::endl;
//        std::cout << "Error!!! Move.from = Move.to = " << (u_int64_t)move.from << std::endl;
//        std::cout << "======================================================" << std::endl;
//        move.from = INVALID_POS;
//        move.to = INVALID_POS;
//        std::cout << "Recearching without tt..." << std::endl;
//        Globals::tt_enabled = false;
//        std::cout << "IT: " << Navi::iterativeDeepening(Globals::_board, ID_WE, 60, 0, &move) << std::endl;
//        std::cout << "MT: " << (u_int64_t)move.from << "->" << (u_int64_t)move.to << std::endl;
//        Globals::tt_enabled = true;
//    }
    
//    std::cout << "Board post calc: " << std::endl;
//    std::cout << "Used:" << std::endl;
//    Tools::printField(_board->used);
//    std::cout << "Mypos:" << std::endl;
//    Tools::printField(_board->mypos);
//    std::cout << "Oppos:" << std::endl;
//    Tools::printField(_board->oppos);
//    std::cout << "movecount: " << _board->movecount << " pointsdiff: " << _board->pointsdiff << std::endl;
    
    if(!BoardTools::isValidMove(Globals::_board, move, ID_WE)){
        std::cout << "Found invalid move: " << ((u_int64_t)move.from) << " -> " << ((u_int64_t)move.to) << " value: " << move.value << std::endl; 
        move = BoardTools::generateGoodMove(Globals::_board, ID_WE);
        std::cout << "New move: " << ((u_int64_t)move.from) << " -> " << ((u_int64_t)move.to) << " value: " << move.value << std::endl; 
    }
    
//    struct timespec nowTime;
//    clock_gettime(Globals::clockTime, &nowTime);
//    
//    while((((nowTime.tv_sec * 1000000000 + nowTime.tv_nsec) - (Globals::moveReqTime.tv_sec * 1000000000 + Globals::moveReqTime.tv_nsec)) / 1000000) < (TIME_FOR_CALC))
//    {
//        clock_gettime(Globals::clockTime, &nowTime);
//    }
    
    Ocean::Send(move);
    BoardTools::apply(&Globals::_board, ID_WE, move);
    sentMove = true;
    
    clock_gettime(Globals::clockTime, &Globals::beginningOther); 

    std::cout << "\n\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% (W) zug nr:  " << Globals::_board.movecount << "\n";
}


/**
 * When getting last move
 * @param move
 */
void onLastMove(Move move)
{
    #ifdef DEBUG_CALLS
        cout << "DEBUG_CALLS: onLastMove called: " << move.from << " -> " << move.to <<endl;
    #endif
    if(sentMove){
        //we sent the last move. It is already apllied to the board in onMoveReq
        #ifdef DEBUG_CALLS
            cout << "DEBUG_CALLS: onLastMove canceled" <<endl;
        #endif
        sentMove = false;
        return;
    }
    
    BoardTools::apply(&Globals::_board, ID_OPPONENT, move);
    
    struct timespec nowTime;
    clock_gettime(Globals::clockTime, &nowTime);
    
    std::cout << "\n\n======================Time of the others (ms): " << (((nowTime.tv_sec * 1000000000 + nowTime.tv_nsec) - (Globals::beginningOther.tv_sec * 1000000000 + Globals::beginningOther.tv_nsec)) / 1000000) << "\n\n\n";

    std::cout << "\n\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% (O) zug nr:  " << Globals::_board.movecount << "\n";
}

int main(int argc, char** argv)
{    
//    Globals::threes = 578994071121432588;
//    Globals::twos = 578994071121432588;
//    Globals::ones = 578994071121432588;
//    Globals::_board = Board();
//    
//    Move move;
//    Evaluation::preEvaluate();
//    clock_gettime(Globals::clockTime, &Globals::moveReqTime); 
//    
//    Move psearchM = startPSearch(5, ID_WE, Globals::_board);
//    std::cout << "PS: " << psearchM.value << std::endl;//change first guess to 100?
//    std::cout << "PM: " << (u_int64_t)psearchM.from << "->" << (u_int64_t)psearchM.to << std::endl;
//
//    std::cout << "IT: " << iterativeDeepening(Globals::_board, ID_WE, 5, 0, &move) << std::endl;//change first guess to 100?
//    std::cout << "MT: " << (u_int64_t)move.from << "->" << (u_int64_t)move.to << std::endl;
//        
//    exit(0);
    
    Globals::tt_enabled = true;
    
    std::cout << "Build: " << build << std::endl << std::endl;
    
    theBreadfish();
    
    usleep(100);
    
    std::cout << "Pointer Limit: " << std::numeric_limits<std::ptrdiff_t>::max() << std::endl;
    
    Fisher::SetDigestion(&onFieldRec, &onLastMove, &onMoveReq);      //set up our fisher
    
    //Ocean::SwimTo("192.168.2.103", "13050", &onPacket, &onTick);
    
    char* host = "127.0.0.1";
    char* port = "13050";
    std::string send("<protocol><joinPrepared reservationCode=\"");
    
    bool reservated = false;
    
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "--host") == 0)host = argv[++i];
        if(strcmp(argv[i], "--port") == 0)port = argv[++i];
        if(strcmp(argv[i], "--reservation") == 0)
        {
            send.append(argv[++i]);
            reservated = true;
        }
        if(strcmp(argv[i], "--points") == 0)Globals::Config::points = atoi(argv[++i]);
        if(strcmp(argv[i], "--rp1") == 0)Globals::Config::reachPoints1 = atoi(argv[++i]);
        if(strcmp(argv[i], "--rp2") == 0)Globals::Config::reachPoints2 = atoi(argv[++i]);
        if(strcmp(argv[i], "--rp3") == 0)Globals::Config::reachPoints3 = atoi(argv[++i]);
//        if(strcmp(argv[i], "--slp1") == 0)Globals::Config::singleLinePoint1 = atoi(argv[++i]);
//        if(strcmp(argv[i], "--slp2") == 0)Globals::Config::singleLinePoint2 = atoi(argv[++i]);
//        if(strcmp(argv[i], "--slp3") == 0)Globals::Config::singleLinePoint3 = atoi(argv[++i]);
        if(strcmp(argv[i], "--alp1") == 0)Globals::Config::allLinePoints1 = atoi(argv[++i]);
        if(strcmp(argv[i], "--alp2") == 0)Globals::Config::allLinePoints2 = atoi(argv[++i]);
        if(strcmp(argv[i], "--alp3") == 0)Globals::Config::allLinePoints3 = atoi(argv[++i]);
        
        if(strcmp(argv[i], "-h") == 0)host = argv[++i];
        if(strcmp(argv[i], "-p") == 0)port = argv[++i];
        if(strcmp(argv[i], "-r") == 0)
        {
            send.append(argv[++i]);
            reservated = true;
        }
    }
    
    Ocean::SwimTo(host, port, &onPacket, &onTick);
    
    if(!reservated)
    {
        Ocean::Send("<protocol><join gameType=\"swc_2015_hey_danke_fuer_den_fisch\"/>");
    }
    else
    {
        send.append("\"/>");
        
        Ocean::Send(send);
    }
    
    Globals::_runningGame = true;
    Globals::_board = Board();
    
    while(Globals::_runningGame && (Globals::_board.movecount <= 60))
    {
        Ocean::GetFood();
    }
    
    
    
    
    int pointsoutside = Tools::popCount(RING1 & Globals::ones);
    pointsoutside += Tools::popCount(RING1 & Globals::twos) * 2;
    pointsoutside += Tools::popCount(RING1 & Globals::threes) * 3;
    std::string stringRingGameData(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING1 & Globals::threes);
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING2 & Globals::ones);
    pointsoutside += Tools::popCount(RING2 & Globals::twos) * 2;
    pointsoutside += Tools::popCount(RING2 & Globals::threes) * 3;
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING2 & Globals::threes);
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING3 & Globals::ones);
    pointsoutside += Tools::popCount(RING3 & Globals::twos) * 2;
    pointsoutside += Tools::popCount(RING3 & Globals::threes) * 3;
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING3 & Globals::threes);
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING4 & Globals::ones);
    pointsoutside += Tools::popCount(RING4 & Globals::twos) * 2;
    pointsoutside += Tools::popCount(RING4 & Globals::threes) * 3;
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    pointsoutside = Tools::popCount(RING4 & Globals::threes);
    stringRingGameData.append(std::to_string(pointsoutside));
    stringRingGameData.append(",");
    
    if(Globals::_board.pointsdiff > 0)
        stringRingGameData.append(",W,");
    else if(Globals::_board.pointsdiff < 0)
        stringRingGameData.append(",L,");
    else if(Globals::_board.pointsdiff == 0)
        stringRingGameData.append(",N,");
    
    stringRingGameData.append(std::to_string(Globals::_board.pointsdiff));
    
    std::cout << "\n\n\nStatistikzeile_Rings: " << stringRingGameData << "\n\n\n";
    
    
    
    std::string cmdToSave("echo \"");
    cmdToSave.append(stringRingGameData);
    cmdToSave.append("\" >> /home/jonas/NetBeansProjectsSC-15-04-24/sc/dist/Release/GNU-Linux-x86/ringtestlog.csv");
    
    const char* cmdToSaveCs = cmdToSave.c_str();
    
    system(cmdToSaveCs);
    
    
    std::cout << std::endl << "Bilance: " << std::endl;
    std::cout << "CutOff: " << Globals::Log::globalCutOff << std::endl;
    std::cout << "Evals:  " << Globals::Log::globalEvalCount << std::endl;
    std::cout << "Nodes:  " << Globals::Log::globalNodesTravled << std::endl << std::endl;
    
    printf("\n\n\n========================================\nRAM-Auslastung:\n========================================\n\n\n");
    
    system("free -m");
    
    sleep(3);
    
    Ocean::GetFood();
    
    sleep(1);
    
    Ocean::Close();
    
#ifdef move_order_stats
    printMoveOrderStats();
    sleep(3);
#endif
    
    return 0;
}
