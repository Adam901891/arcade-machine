#include "AboutScreen.h"
#include "Configuration.h"
#include "splashkit.h"
#include <string>
#include <cmath>
#include <vector>

#define TITLE_FONT_SIZE 44
#define TITLE_FONT_CHAR_WIDTH 32
#define TITLE_FONT_CHAR_HEIGHT 32
#define TITLE_FONT_Y ((((ARCADE_MACHINE_RES_Y / 2) + (TITLE_FONT_CHAR_HEIGHT / 2))) / 3)
#define STAR_COUNT 1024
#define DISTANCE_SHIFT 10

const char *title = "About The Thoth Tech Arcade Machine!";

AboutScreen::AboutScreen() {
	this->m_shouldQuit = false;
	this->m_titleX = ARCADE_MACHINE_RES_X;
	this->m_title = std::string(title);
	this->m_titleEnd = ((TITLE_FONT_CHAR_WIDTH * this->m_title.length()));
	this->m_titleEnd *= -1;
	this->m_stars = std::vector<struct s_star>();

	for (int i=0; i<STAR_COUNT; ++i) {
		struct s_star star;
		star.x = (rand() % (ARCADE_MACHINE_RES_X - 0 + 1)  + 0);
		star.y = (rand() % (ARCADE_MACHINE_RES_Y - 0 + 1) + 0);
		star.distance = (rand() % (DISTANCE_SHIFT - 1 + 1) + 1);

		double brightness = (rand() % (80 - 40) + 40);
		brightness /= 100;

		star.c.a = brightness;
		star.c.r = 1;
		star.c.g = 1;
		star.c.b = 1;

		this->m_stars.push_back(star);
	}

}

void AboutScreen::readInput() {
	if (quit_requested() || key_down(ESCAPE_KEY))
		this->m_shouldQuit = true;
}

void AboutScreen::tick() {
	this->shiftTitle();
	this->shiftStars();

	this->m_ticker++;
}

void AboutScreen::render() {
	clear_screen(COLOR_BLACK);

	this->renderTitle();
	this->renderStars();

	refresh_screen();
}

void AboutScreen::shiftTitle() {
	this->m_titleX -= 6;

	if (this->m_titleX < this->m_titleEnd)
		this->m_titleX = ARCADE_MACHINE_RES_X;
}

void AboutScreen::renderTitle() {
	double x = this->m_titleX;
	static font f = font_named("upheavtt.ttf");

	for (int i=0; i<this->m_title.length(); ++i) {
		double y = TITLE_FONT_Y + (sin(x / 80) * 115);
		double fontSize = TITLE_FONT_SIZE + (sin(x / 120) * 12);
		color c;
		c.r = sin(i + 0) * 127 + ((sin(x / 80)) * 1.28);
		c.g = sin(i + 2) * 127 + ((sin(x / 80)) * 1.28);
		c.b = sin(i + 4) * 127 + ((sin(x / 80)) * 1.28);


		// Underlay.
		draw_text(
			this->m_title.substr(i, 1), 
			COLOR_GRAY,
			f,
			fontSize,
			x + 2,
			y + 2
		);	

		draw_text(
			this->m_title.substr(i, 1), 
			c,
			f,
			fontSize,
			x,
			y
		);	

		x += TITLE_FONT_CHAR_WIDTH;
	}
}

void AboutScreen::shiftStars() {
	for (int i=0; i<this->m_stars.size(); ++i) {
		this->m_stars[i].x += this->m_stars[i].distance;
		if (this->m_stars[i].x > ARCADE_MACHINE_RES_X)
			this->m_stars[i].x = -10;
	}
}

void AboutScreen::renderStars() {
	for (auto star : this->m_stars)
		fill_rectangle(star.c, star.x, star.y, star.distance / 2, star.distance / 4);
}

void AboutScreen::loop() {
	while (! quit_requested()) {
		process_events();

		this->readInput();
		this->tick();
		this->render();

		delay(1000 / 60);
	}
}