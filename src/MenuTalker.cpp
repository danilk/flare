/**
 * class MenuTalker
 *
 * @author morris989 and Clint Bellanger
 * @license GPL
 */

#include "MenuTalker.h"

MenuTalker::MenuTalker(SDL_Surface *_screen, FontEngine *_font, CampaignManager *_camp) {
	screen = _screen;
	font = _font;
	camp = _camp;
	npc = NULL;
	
	visible = false;

	// step through NPC dialog nodes
	dialog_node = 0;
	event_cursor = 0;
	accept_lock = false;

	loadGraphics();

}

void MenuTalker::loadGraphics() {

	background = IMG_Load("images/menus/dialog_box.png");
	if(!background) {
		fprintf(stderr, "Couldn't load image dialog_box.png: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
	
}

void MenuTalker::chooseDialogNode() {
	event_cursor = 0;
	dialog_node = npc->chooseDialogNode();
	npc->processDialog(dialog_node, event_cursor);
}

/**
 * Menu interaction (enter/space/click to continue)
 */
void MenuTalker::logic(bool pressing_accept) {

	if (!visible || npc==NULL) return;
	
	bool more;
	
	if (pressing_accept && accept_lock)
		return;
	else if (!pressing_accept) {
		accept_lock = false;
		return;
	}
	else accept_lock = true;
	
	// pressed next/more
	event_cursor++;
	more = npc->processDialog(dialog_node, event_cursor);
	
	if (!more) {
		// end dialog
		npc = NULL;
		visible = false;
	}
}

void MenuTalker::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;
	string line;
	
	int offset_x = (VIEW_W - 640)/2;
	int offset_y = (VIEW_H - 416)/2;
	
	// dialog box
	src.x = 0;
	src.y = 0;
	dest.x = offset_x;
	dest.y = offset_y + 320;
	src.w = dest.w = 640;
	src.h = dest.h = 96;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// show active portrait
	string etype = npc->dialog[dialog_node][event_cursor].type;
	if (etype == "him" || etype == "her") {
		if (npc->portrait != NULL) {
			src.w = dest.w = 320;
			src.h = dest.h = 320;
			dest.x = offset_x + 48;
			dest.y = offset_y;
			SDL_BlitSurface(npc->portrait, &src, screen, &dest);	
		}
		line = npc->name + ": ";
	}
	else if (etype == "you") {
		// TODO: display the player's chosen portrait
		// TODO: display the player's chosen name
		line = "You: ";
	}
	
	// text overlay
	line = line + npc->dialog[dialog_node][event_cursor].s;
	font->render(line, offset_x+48, offset_y+336, JUSTIFY_LEFT, screen, 544, FONT_WHITE);
}


MenuTalker::~MenuTalker() {
	SDL_FreeSurface(background);
}
