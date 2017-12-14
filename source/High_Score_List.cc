/*
 * High_Score_List.cc
 */

#include <SFML/Graphics.hpp>
#include "High_Score_List.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

High_Score_List::High_Score_List()
  : window(sf::VideoMode(1280, 720), "Highscore List"),
    splash_screen{Button(12,12,0,0,"resources/highscores.png")},
  return_button{Button(12,12,0,0,"resources/button_quit.png")} // Placeholder icon for "Return"
{
  window.setFramerateLimit(60);
}

void High_Score_List::run()
{
  while( window.isOpen() )
    {
      process_events();
      render();
    }

  sf::Text text;
  text.setString("Hello world");
}

void High_Score_List::process_events()
{
  sf::Event event;
  while (window.pollEvent(event))
    {
      switch(event.type)
	{
	case sf::Event::Closed:
	  {
	    return;
	  }
	case sf::Event::KeyPressed:
	  if (event.key.code == sf::Keyboard::Escape) 
	    {
	      window.close();
	    } 
	case sf::Event::MouseButtonPressed:
	  {
	    if (event.mouseButton.button == sf::Mouse::Left)
	      {
		if(sf::Mouse::getPosition(window).x > 1 && sf::Mouse::getPosition(window).x < 100 &&
		   sf::Mouse::getPosition(window).y > 0 && sf::Mouse::getPosition(window).y < 100)
		  {
		    window.close();
		    Start_Menu start_menu {};
		    string user_choice = start_menu.run();
		    Game game {};
		    game.run(user_choice);
		  }
	      }
	  break;
	  }
    default:
      break;
	}
    }
}

void High_Score_List::render()
{
  splash_screen.get_sprite().setPosition(560,300);
  return_button.get_sprite().setPosition(1,1);
  window.clear(sf::Color(10,110,191));
  window.draw(splash_screen.get_sprite());
  window.draw(return_button.get_sprite());
  window.draw(text);
  window.display();
}


void High_Score_List::add_entry(unique_ptr<Entry> entry)
{
  int score = entry->get_score();

  for(unsigned i{} ; i < entries.size() ; i++)
    {
      int current_score {};
      current_score = entries.at(i)->get_score();
      if(score > current_score)
	{
	  entries.insert(entries.begin()+i, move(entry));
	  if(entries.size() > 10) 
	    {
	      entries.pop_back();
	    }
	}
    }
}

void High_Score_List::show_score()
{
  save_to_file();
  ifstream infile;
  string line;
  infile.open("resources/Highscorelist.txt");
  getline(infile,line);
  cout << line << endl;
  /*
  for( unsigned i{} ; i < entries.size() ; i++ )
    {
      if(infile.is_open())
	{
	  getline(infile,line);
	  cout << line << endl;
	}
    }
  */
  infile.close();
}

void High_Score_List::save_to_file()
{
  for(unsigned i{} ; i < entries.size() ; i++)
    {
      string new_entry = entries.at(i)->to_string();
      ofstream outfile;
      outfile.open("resources/Highscorelist.txt");
      outfile << new_entry;
      outfile.close();
    }

  // For testing purposes
  ofstream outfile;
  outfile.open("resources/Highscorelist.txt");
  string test = to_string();
  outfile << test;
  outfile.close();
}

