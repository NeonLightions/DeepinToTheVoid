#include "Logging.h"
#include "TitleFont.h"
#include "curses.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#ifdef _WIN32
#undef KEY_ENTER
#define KEY_ENTER 10
#endif

bool g_isDebugEnabled = false;
std::atomic<bool> g_isRunning = true;

class Game {
public:
    Game() {
        initscr(); // Initialize the screen
        noecho(); // Disable echoing input
        curs_set(0); // Hide the cursor

        m_mainWindow = newwin(40, 160, 0 ,0);
        if (!m_mainWindow) {
            LOG(critical) << "Failed to initialize m_mainWindow";
            return;
        }

        m_introWindow = newwin(40, 160, 0, 0);
        if (!m_introWindow) {
            LOG(critical) << "Failed to initialize m_introWindow";
            return;
        }

        keypad(stdscr, true);
        keypad(m_mainWindow, true);
        keypad(m_introWindow, true);

        nodelay(stdscr, true);
        nodelay(m_mainWindow, true);

        wtimeout(m_mainWindow, 100);

        if (has_colors()) { 
            start_color();
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
        } else {
            LOG(critical) << "Color unsupported";
        }

        m_currentWindow = m_mainWindow;
        m_inputHandler = std::thread(&Game::handleInput, this);
        m_inputHandler.detach();
        m_refreshHandler = std::thread(&Game::keepUpdating, this);
        m_refreshHandler.detach();

        LOG(info) << "Console initialized successfully";

    }

    ~Game() {
        delwin(m_mainWindow);
        delwin(m_introWindow);
        endwin();
    }

    void startGame() {
        while (g_isRunning) {
            mainScreen();
            introductionScreen();
        }
    }

    void mainScreen() {
        {
            std::lock_guard<std::mutex> mainWindowLock(m_windowMtx);
            wborder(m_mainWindow, 186, 186, 205, 205, 201, 187, 200, 188);
            
            // Print title
            int currentCol = 7;
            int currentLine = 3;
            for (char c : "Deepin To The Void") {
                for (const std::string& line : s_titleFont[c]) {
                    mvwaddstr(m_mainWindow, currentLine, currentCol, line.c_str());
                    currentLine++;
                }
                currentCol += s_CharWidth + 1;
                currentLine = 3;
            }
            m_currentWindow = m_mainWindow;
        }
        m_choiceInMainScreen = 0;

        mainWindowDrawChoices();
        wnoutrefresh(m_mainWindow);

        while (g_isRunning) {
            getInput();
        
            bool isPlayerChoosed = false;
            switch (m_inputKey) {
                case KEY_DOWN: {
                    if (m_choiceInMainScreen < 2) { m_choiceInMainScreen++; }
                    break;
                }
                case KEY_UP: {
                    if (m_choiceInMainScreen > 0) { m_choiceInMainScreen--; }
                    break;
                }
                case KEY_ENTER: {
                    LOG(info) << "Player choosed: " << m_choicesInMainScreen[m_choiceInMainScreen];
                    isPlayerChoosed = true;
                    break;
                }
            }
            m_isInputReceived = false;

            if (isPlayerChoosed) {
                {
                    std::lock_guard<std::mutex> winLock(m_windowMtx);
                    wclear(m_currentWindow);
                }
                break;
            }
        
            mainWindowDrawChoices();
            wnoutrefresh(m_mainWindow);
        }
    }

    void introductionScreen() {
        {
            std::lock_guard<std::mutex> winLock(m_windowMtx);
            m_currentWindow = m_introWindow;
        }

        wborder(m_currentWindow, 186, 186, 205, 205, 201, 187, 200, 188);
        mvwaddstr(m_currentWindow,  3, 74, "Introduction");
        mvwaddstr(m_currentWindow, 37, 68, "Press Enter to continue.");

        mvwaddstr(m_currentWindow, 5, 10, "EXO: The Endless Voyage");

        mvwaddstr(m_currentWindow, 7, 10, "You are the commander of EXO, a spacecraft built for a one-way journey into the unknown. There is no return mission, no");
        mvwaddstr(m_currentWindow, 8, 10, "depends entirely on your ability to manage the ship.");
        
        mvwaddstr(m_currentWindow, 10, 10, "Before even leaving Earth's orbit, you must:");
        mvwaddstr(m_currentWindow, 11, 10, "    - Run full system diagnostics - Check power distribution, fuel levels, and onboard computing integrity.");
        mvwaddstr(m_currentWindow, 12, 10, "    - Secure cargo and supplies - Ensure life support has enough oxygen, food rations are sufficient,");
        mvwaddstr(m_currentWindow, 13, 10, "      sand spare parts are stocked.");
        mvwaddstr(m_currentWindow, 14, 10, "    - Verify navigation systems - Program initial coordinates and confirm long-range scanning is functional.");
        mvwaddstr(m_currentWindow, 15, 10, "    - Perform emergency drills - Simulate fires, system failures, and hull breaches to test response protocols.");

        mvwaddstr(m_currentWindow, 17, 10, "Once in the void, the real challenge begins. Every action is controlled via console commands, with no graphical interface-only logs,"); 
        mvwaddstr(m_currentWindow, 18, 10, "alerts, and system feedback.");

        mvwaddstr(m_currentWindow, 20, 10, "Are you prepared to venture into the void?");

        // Nothing change in this window, so don't need to 
        // perodically refresh it.
        wnoutrefresh(m_currentWindow);

        while (g_isRunning) {
            getInput();
            if (m_inputKey == KEY_ENTER) {
                m_isInputReceived = false;
                break;
            }

            m_isInputReceived = false;
        }
    }

private:
    WINDOW *m_introWindow;    
    WINDOW *m_mainWindow;

    std::mutex m_inputMtx;
    std::mutex m_refreshMtx; 
    std::mutex m_windowMtx;

    std::condition_variable m_cv;
    std::condition_variable m_cvForInput;

    std::atomic<int>     m_inputKey;
    std::atomic<bool>    m_isInputReceived;
    std::atomic<bool>    m_isNeedUpdate;
    std::atomic<WINDOW*> m_currentWindow;

    std::thread m_inputHandler;
    std::thread m_refreshHandler;

    int m_choiceInMainScreen;

    const std::string m_choicesInMainScreen[3] = {
        "Start game",
        "  Option  ",
        "   Quit   "
    };

    void mainWindowDrawChoices() {
        std::lock_guard<std::mutex> mainWindowLock(m_windowMtx);
        for (int i = 0; i < 3; i++) {
            if (i == m_choiceInMainScreen) {
                wattron(m_mainWindow, COLOR_PAIR(1));
                mvwaddstr(m_mainWindow, 16 + (i * 2), 
                    (160 - m_choicesInMainScreen[i].length()) / 2, 
                    m_choicesInMainScreen[i].c_str());
                wattroff(m_mainWindow, COLOR_PAIR(1));
            } else {
                mvwaddstr(m_mainWindow, 16 + (i * 2), 
                    (160 - m_choicesInMainScreen[i].length()) / 2, 
                    m_choicesInMainScreen[i].c_str());
            }
        }
    }

    void keepUpdating() {
        while (g_isRunning) {
            {
                std::lock_guard<std::mutex> refreshLock(m_refreshMtx);
                doupdate();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void handleInput() {
        while (g_isRunning) {
            int key;    
            {
                std::lock_guard<std::mutex> winLock(m_windowMtx);
                key = wgetch(m_currentWindow);
            }

            if (key == ERR) { continue; }
    
            if (key == 27) { // ESC key
                g_isRunning = false;
                m_cvForInput.notify_all();
                break;
            }
    
            { 
                std::lock_guard<std::mutex> inputLock(m_inputMtx);
                m_inputKey = key;
                m_isInputReceived = true;
            }
    
            m_cvForInput.notify_all();
    
            // Chờ xử lý xong input
            while (m_isInputReceived.load()) {
                std::this_thread::yield();
            }
        }
    }

    void getInput() {
        while (!m_isInputReceived.load() && g_isRunning) {
            std::unique_lock<std::mutex> inputLock(m_inputMtx);
            if (!m_cvForInput.wait_for(inputLock, 
                                std::chrono::milliseconds(100), 
                                [this] { 
                                        return m_isInputReceived.load() || !g_isRunning; 
                                })) {
                continue;
            }
        }
    }
};

int main(int argc, char** argv)
{
    /** For debug mode: .\Deepin.exe debug */
    if (argc > 1 && !strncmp(argv[1], "debug", 5)) {
        g_isDebugEnabled = true;
    }

    Logger::init();
    Game game;
    game.startGame();

    getch();
    endwin();
    return 0;
}
