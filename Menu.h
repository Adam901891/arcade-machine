#ifndef ARCADE_MACHINE_MENU_H
#define ARCADE_MACHINE_MENU_H

#include "Tip.h"
#include "Selector.h"
#include "GameData.h"
#include "Database.h"
#include "Rating.h"
#include "Table.h"

#ifdef _WIN32
#include <Windows.h>
#endif

class Menu {
private:
    std::string background = "games_dashboard";
    // Vector to store the config data of each game
    std::vector<ConfigData> _games;
    Database *_db;
    Rating _rating;
    GameData _gameData;

#ifdef _WIN32
    // Contains info about newly created process and thread
    PROCESS_INFORMATION processInfo;
    // Unsigned int to store exit info
    DWORD exit_code;
    // Holds the game path of selected game
    LPCSTR _gamePath;
    // Holds the executable of selected game
    LPSTR _gameExe;
    // Holds the game directory of selected game
    LPCSTR _gameDir;
#endif

    // Used to find x centre of screen
    double _center_x = 960;
    // Used to fine y centre of screen
    double _center_y = 540;
    // Stores x of mouse position
    double _x;
    // Stores y of mouse position
    double _y;
    // Checks if game is running
    bool _in_game = false;
    // Checks if program has exited
    bool _program_exit;
    // Vector of buttons
    std::vector<Button*> btns;
    // Vectore to store game images
    std::vector<std::string> game_images;
    // Menu grid
    Grid _grid;
    Tip *tip;
    ButtonNode *button = nullptr;
    bool _overlayActive = false;
    /// Button Action
    std::string _action;
    Selector _selector_games_menu;
    // Passes into Selector optional parameter.
    bool game_menu = true;

#ifdef _WIN32
    // Handle for game window.
    HWND handle;
#endif

    // Determines when game has started.
    bool _game_started = false;
    // Starting position of button x.
    const int position = 700;
    // Position of button y.
    const int _posY = 270;
    // Increments x of sprite.
    const int speed = 30;
    // The positions of the sprites for slide.
    int _pos1 = position;
    int _pos2 = _pos1 - position;
    int _pos3 = _pos2 - position;
    int _pos4 = position * 2;
    int _pos5 = position * 3;
    // Hold the button sprites for slide.
    sprite _new_button1;
    sprite _new_button2;
    sprite _new_button3;
    // Determines when sliding.
    bool _menu_sliding = false;

    // Creates a new game data object.
    GameData *m_newGame = new GameData;

public:
    Menu(){
        this->_db = new Database();
        Table *gameDataTable = new Table("gameData", {{"gameName", "TEXT"}, {"startTime", "TEXT"}, {"endTime", "TEXT"}, {"rating", "TEXT"}, {"highScore", "TEXT"}});
        this->_db->createTable(gameDataTable);
        this->_rating = Rating();
    }

    Menu(std::vector<ConfigData> configs)
    {
        this->_games = configs;
        this->_db = new Database();
        Table *gameDataTable = new Table("gameData", {{"gameName", "TEXT"}, {"startTime", "TEXT"}, {"endTime", "TEXT"}, {"rating", "TEXT"}, {"highScore", "TEXT"}});
        this->_db->createTable(gameDataTable);
        this->_rating = Rating();


#ifdef _WIN32
        handle = FindWindowA(NULL, "arcade-machine");
#endif
    }
    ~Menu(){}

    // Getters
    auto get_buttons() const -> const std::vector<Button*> { return this->btns; }
    bool get_overlay_state() { return _overlayActive; }

    /** 
     * @brief Gets the game images from the config files and returns vector of game images.
     * 
     * @param configs Vector of config data.
     * @return vector of game images.
     */
    std::vector<std::string> get_game_sprites(std::vector<ConfigData> configs)
    {
        std::vector<std::string> game_images;

        for (int i = 0; i < configs.size(); i++)
        {
            // Get image dir and image name from games vector.
            std::string image = configs[i].folder() + "/" + configs[i].image();
            game_images.push_back(image);
        }

        return game_images;
    }


    /**
     * @brief Create a grid object
     */
    void create_grid()
    {
        // Instantiate grid object
        Grid grid(8, 14);
        this->_grid = grid;
        // Update the background
        this->_grid.SetBackground(bitmap_named(this->background));
    }

    /**
     * @brief Create a list of games.
     * 
     */
    void create_buttons()
    {
        // Call function to get game images.
        game_images = get_game_sprites(_games);
        GameData gameStats;
        for (int i = 0; i < game_images.size(); i++)
        {
            if (i == 0)
            {
                this->button = new ButtonNode(new GameScreenButton(Button::GAME, game_images[0]));
                this->button->config = _games[0];
                this->button->stats = gameStats.getStats(this->_db, _games[0].title());
            }
            else
            {
                std::string image = game_images[i];
                this->button->addBefore(new ButtonNode(new GameScreenButton(Button::GAME, image)));
                this->button->getPrev()->config = _games[i];
                this->button->getPrev()->stats = gameStats.getStats(this->_db, _games[i].title());
            }
        }
    }

    /** 
     * @brief create a tip to display to the user.
     */
    void create_tip()
    {
        bitmap bmpTip = bitmap_named("information");
        //Breakdown the sheet
        bitmap_set_cell_details(bmpTip, 50, 50, 4, 3, 12);
        //Fetch the animation script
        animation_script info_script = animation_script_named("info-script");
        //Create the animation
        animation anim = create_animation(info_script, "rotate");
        //Load the animation into options
        drawing_options opt = option_with_animation(anim);
        //Create the tip
        std::string tip_text[3] = {"Use the left and right arrow keys to cycle through the carousel", "Press escape to return to the main menu", "Press enter to start the game"};
        this->tip = new Tip(tip_text[rand()%3],bmpTip, anim, opt, 3000, 25);
    }

    /**
     * @brief draw the game buttons to the window, using the carousel layout
     */
    void update_carousel()
    {
        // If menu is sliding then clear the grid.
        if (this->_menu_sliding)
        {
            this->_grid.ClearGrid();
        }
        else {
            if (this->button && !this->_in_game)
            {
                this->_grid.UpdateCell(this->button->getPrev()->button, 2, 0, 1, false);
                this->_grid.UpdateCell(this->button->button, 2, 5, 1, false);
                this->_grid.UpdateCell(this->button->getNext()->button, 2, 10, 1, false);
            }
        }
    }

    /**
     * @brief handle carousel input
     */
    void carousel_handler()
    {
        /// Check for input in selector class.
        this->button = this->_selector_games_menu.check_key_input(this->button, game_menu);
        this->_action = this->_selector_games_menu.check_for_selection(this->button, game_menu);

#ifdef _WIN32
        check_game_exit();
#endif

        if (this->button)
        {
            if (this->_action == "escape" && _overlayActive)
            {
                _overlayActive = false;
            }
            else if (this->_action == "return")
            {
                if (_overlayActive)
                {
#ifdef _WIN32
                    // Get game path
                    _gamePath = (this->button->config.folder() + "/" + this->button->config.exe()).c_str();
                    // Get executable name
                    _gameExe = strdup(this->button->config.exe().c_str());
                    // Get game directory
                    _gameDir = this->button->config.folder().c_str();
#endif

                    // Set the center of the game
                    this->_x = _center_x;
                    this->_y = _center_y;

                    // fade to black
                    fade(0, 1, 0.1);

                    // fill with black
                    fill_rectangle(rgba_color(0.0, 0.0, 0.0, 1.0), 0, 0, 1920, 1080);
                    // clear grid
                    this->_grid.ClearGrid();
                    // set new background
                    this->_grid.SetBackground(bitmap_named("in_game_bgnd"));
                    //turn off overlay
                    this->_overlayActive = false;
                    // turn off menu music
                    fade_music_out(1000);
                    // fade back in
                    fade(1, 0, 0.1);

#ifdef _WIN32
                    _gameData.setStartTime(time(0));
                    _gameData.setGameName(this->button->config.title());
                    // Call method to open game executable
                    start_game(_gamePath, _gameExe, _gameDir);
#endif

                    return;
                }
                _overlayActive = true;
            }
        }
    }

    /**
     * @brief draw the menu page
     */
    void draw_menu_page()
    {
        // if the game has ended, go back to games menu
        if(!this->_in_game && this->_game_started)
        {
            this->_game_started = false;
            back_to_games_menu();
            _gameData.setRating(_rating.getRating());
            _gameData.writeData(this->_db);
            this->button->stats = _gameData.getStats(this->_db, _gameData.getGameName());
        }
        
        this->_grid.DrawGrid();
        
        // Wait for selector to key input to determine slide direction.
        if (_selector_games_menu.get_slide_left())
            draw_update_slide_left();
        else if (_selector_games_menu.get_slide_right())
            draw_update_slide_right();

        if (_overlayActive && !_menu_sliding)
            draw_overlay(button->config, button->stats);
        if (!_in_game)
            this->tip->draw();

        update_carousel();
        carousel_handler();
    }

    /**
     * @brief Method to update the sprite positions and draw sprite.
     * 
     * @param button_sprite The buttons sprite.
     * @param position The position to move the sprite.
     * @return ** void 
     */
    void update_slide(sprite button_sprite, int position)
    {
        // Show the base layer of sprite.
        sprite_show_layer(button_sprite, 0);
        // Set the x position of sprite.
        sprite_set_x(button_sprite, position);
        // Set the y position of sprite.
        sprite_set_y(button_sprite, _posY);
        // draw sprite to screen.
        draw_sprite(button_sprite);
        // Updatse sprite.
        update_sprite(button_sprite);
    }

    /**
     * @brief Slide the game buttons on left key input.
     * 
     * @return ** void 
     */
    void draw_update_slide_left()
    {
        this->_menu_sliding = true;

        // Get sprites of buttons on display.
        this->_new_button1 = this->button->getNext()->button->btn();
        this->_new_button2 = this->button->button->btn();
        this->_new_button3 = this->button->getPrev()->button->btn();

        // Increment the x position of sprite.
        this->_pos1 += speed;
        this->_pos2 += speed;
        this->_pos3 += speed;

        // Update and draw sprite.
        update_slide(this->_new_button1, this->_pos1);
        update_slide(this->_new_button2, this->_pos2);
        update_slide(this->_new_button3, this->_pos3);

        // If sprite reaches position.
        if (this->_pos1 > 1300)
        {
            // Set selector bool back to false.
            _selector_games_menu.set_slide_left(false);
            this->_menu_sliding = false;
            // Reset positions.
            this->_pos1 = position;
            this->_pos2 = this->_pos1 - position;
            this->_pos3 = this->_pos2 - position;
        }
    }

    /**
     * @brief Slide the game buttons on right key input.
     * 
     * @return ** void 
     */
    void draw_update_slide_right()
    {
        this->_menu_sliding = true;

        // Get sprites of buttons on display.
        this->_new_button1 = this->button->getPrev()->button->btn();
        this->_new_button2 = this->button->button->btn();
        this->_new_button3 = this->button->getNext()->button->btn();

        // Decrease the x position of sprite.
        this->_pos1 -= speed;
        this->_pos4 -= speed;
        this->_pos5 -= speed;

        // Update and draw sprite.
        update_slide(this->_new_button1, this->_pos1);
        update_slide(this->_new_button2, this->_pos4);
        update_slide(this->_new_button3, this->_pos5);

        if (this->_pos1 <= 20)
        {
            // Set selector bool back to false.
            _selector_games_menu.set_slide_right(false);
            this->_menu_sliding = false;
            // Reset positions.
            this->_pos1 = position;
            this->_pos4 = position * 2;
            this->_pos5 = position * 3;
        }
    }
    /**
     * @brief Draw an overlay over the game, using data from the config.
     * 
     * @param config the game config.
     */
    void draw_overlay(ConfigData config, GameData stats)
    {
        int x_offset = (current_window_width() / 2) + (current_window_width() / 14);
        int y_start = current_window_height() / 6;
        int y_offset = current_window_height() / 40;

        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 0.8), (current_window_width() / 2), 0, (current_window_width() / 2), current_window_height());
        draw_text(config.title(), COLOR_WHITE, "font_title", y_offset * 3, x_offset, y_start);
        y_start += y_offset * 3;
        draw_text("Author: " + config.author(), COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (1 * y_offset));
        draw_text("Genre: " + config.genre(), COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (2 * y_offset));
        draw_text("Language: " + config.language(), COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (3 * y_offset));
        draw_text("Classification: " + config.rating(), COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (4 * y_offset));
        draw_text("Repository: " + config.repo(), COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (5 * y_offset));
        draw_text("Rating: ", COLOR_WHITE, "font_text", y_offset, x_offset, y_start + (6 * y_offset));
        string rating = string(stats.getRating(), 'J');
        rating += string(5-rating.size(),'I');
        draw_text(rating, COLOR_WHITE, "font_star", y_offset+10, x_offset + 90,  y_start + (6 * y_offset));
    }

#ifdef _WIN32
    /**
     * @brief  Find the game window and bring it to focus, if it exists
     * 
     * @param windowName the name of the window
     * @param timeout time in ms to search for the window
     * @return true/false if window was found.
     */
    bool FocusWindow(std::string windowName, int timeout = 2000)
    {
        LPCSTR gameWindow =  windowName.c_str();
        HWND gameWindowHandle = NULL;

        int timeElapsed;
        auto startTime = chrono::steady_clock::now();

        //Find the window handle
        do {
            gameWindowHandle = FindWindowEx(NULL,NULL,NULL, gameWindow);
            timeElapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
            delay(250);
        }
        while (gameWindowHandle == NULL && timeElapsed <= timeout);

        //Maximise the Window
        if (gameWindowHandle != NULL)
        {
            ShowWindow(gameWindowHandle, SW_SHOWMAXIMIZED);
            return true;
        }
        else
        {
            write_line("Unable to find gameWindow Handle");
            return false;
        }
        return true;
    }
#endif

#ifdef _WIN32
    /**
     * @brief Starts up the selected game by starting a new process.
     * 
     * @param gamePath The filepath of the game to open.
     * @param gameExe The executable of the game.
     * @param gameDirectory // The directory of the game.
     * @return ** void 
     */
    void start_game(LPCSTR gamePath,LPSTR gameExe, LPCSTR gameDirectory)
    {
        if (!this->_in_game)
        {
            // Additional info
            STARTUPINFOA startupInfo;

            // Set the size of the structures
            ZeroMemory(&startupInfo, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);
            ZeroMemory(&processInfo, sizeof(processInfo));

            // Start the program up
            WINBOOL gameProcess = CreateProcessA
            (
                gamePath,               // the path
                gameExe,                // Command line
                NULL,                   // Process handle not inheritable
                NULL,                   // Thread handle not inheritable
                FALSE,                  // Set handle inheritance to FALSE
                NORMAL_PRIORITY_CLASS,     // Don't open file in a separate console
                NULL,                    // Use parent's environment block
                gameDirectory,           // Use parent's starting directory
                &startupInfo,            // Pointer to STARTUPINFO structure
                &processInfo           // Pointer to PROCESS_INFORMATION structure
            );

            OpenProcess(PROCESS_QUERY_INFORMATION,TRUE, gameProcess);

            std::string windowName = gameExe;
            //Remove the extension from the application name (.exe)
            windowName = windowName.substr(0, windowName.find("."));
            //Focus the window
            FocusWindow(windowName);

            this->_in_game = true;
        }
    }
#endif
   
#ifdef _WIN32
   /**
     * @brief Waits for game to exit.
     * 
     * @return ** void 
     */
    void check_game_exit()
    {
        if (this->_in_game == true)
        {
            this->_game_started = true;
            // Check if game has been exited.
            this->_program_exit = GetExitCodeProcess(processInfo.hProcess, &exit_code);
            if ((this->_program_exit) && (STILL_ACTIVE != exit_code))
            {
                this->_in_game = false;

                int highScore = 0;

                _gameData.setEndTime(time(0));
                _gameData.setHighScore(highScore);
            }
        }
    }
#endif

    /** 
     * @brief Fade back to games menu
     */
    void back_to_games_menu()
    {
        // fade to black
        fade(0, 1, 0.1);
        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 1.0), 0, 0, 1920, 1080);
        this->_grid.SetBackground(bitmap_named("games_dashboard"));
        // fade to normal
        fade(1, 0, 0.1);
    }

    /**
     * @brief Creates a fading effect
     * 
     * @param alphaStart The starting alpha value.
     * @param alphaEnd The ending alpha value.
     * @param alphaStep The alpha value to increment/decrement by.
     */
    void fade(double alphaStart, double alphaEnd, double alphaStep)
    {
        if (alphaStart > alphaEnd)
            alphaStep = -abs(alphaStep);
        // Calculate the number of steps required to complete the fade.
        double difference = abs(alphaEnd - alphaStart);
        int steps = difference / abs(alphaStep);

        for (int i = 0; i < steps; i++)
        {
            clear_screen();
            this->_grid.DrawGrid();
            // Alpha value manipulates to the opacity of the rectangle.
            fill_rectangle(rgba_color(0.0, 0.0, 0.0, alphaStart), 0, 0, 1920, 1080);
            // Update the alpha value.
            alphaStart += alphaStep;
            refresh_screen(60);

            delay(50);
        }
    }
};

#endif
