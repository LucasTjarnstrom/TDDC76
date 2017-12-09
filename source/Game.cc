/*
 * Game.cc
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <exception>
#include <memory>

#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Floor.h"
#include "Wall.h"
#include "Collision.h"
#include "High_Score_List.h"

using namespace std;

Game::Game()
    : window(sf::VideoMode(1280, 720), "SFML works!"),
      player{Player(70,0,0,0,"resources/player.png")}
{
    window.setFramerateLimit(60); // FPS set to 60
    
    unique_ptr<Floor> temp1 = make_unique<Floor>(0,400,0,0,"resources/floor2.png");
    map.get_environments().push_back(move(temp1));
    unique_ptr<Wall> temp2 = make_unique<Wall>(0,-100,0,0,"resources/wall2.png");
    map.get_environments().push_back(move(temp2));
}

void Game::run(string user_choice)
{   
  if ( user_choice == "Start game" )
    {
      while (window.isOpen())
	{
	  process_events();
	  update();
	  render();
	}   
    }

  else if ( user_choice == "Continue" )
    {
      cout << "Continue game" << endl;
      return;
    }

  else if ( user_choice == "Show high scores" )
    {
      window.close();
      High_Score_List high_score_list {};
      high_score_list.run();
    }
  
  else if(user_choice == "Quit" )
    {
      return;
    }
  else
    {
	throw logic_error("Start_Menu returns invalid string!");
      return;
    }
}

void Game::process_events()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
	switch(event.type)
	{
	    //windows closed
	case sf::Event::Closed:
	    window.close();
	    
	case sf::Event::KeyPressed:
	    if (event.key.code == sf::Keyboard::Escape) //press escape -> close window
	    {
	    	window.close();
	    } else
		handle_player_input(event.key.code, true);
	    break;
	    
	case sf::Event::KeyReleased:
	    handle_player_input(event.key.code, false);
	    break;
	    	    
	case sf::Event::MouseMoved:
	{
	    //std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
	    //std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
	    break;
	}
	default:
	    break;	
	}
    }
    
}

void Game::update()
{
    if(move_left)
	movement = "left";
    else if(move_right)
	movement = "right";
    else
	movement = "nothing";
  
    player.move(movement);

    //cout << "Physics updated" << endl;
   
}

void Game::render()
{

  if (Collision::BoundingBoxTest(player.draw_this(), map.get_environments().front()->draw_this()) &&
	Collision::BoundingBoxTest(player.draw_this(), map.get_environments().back()->draw_this()))
    {
	player.set_y_velocity(-0.1);
	cout << "Floor & Wall Collision!!" << endl;
	player.set_x_velocity(12); // hur ska detta funka?
    } else if(Collision::BoundingBoxTest(player.draw_this(), map.get_environments().front()->draw_this()))
    {
	player.set_y_velocity(-0.1);
	player.jump_counter = 1;
    }else if(Collision::BoundingBoxTest(player.draw_this(), map.get_environments().back()->draw_this()))
    {
        cout << "Wall Collision!!" << endl;
	player.set_x_velocity(12); // hur ska detta funka?
    }

    window.clear(sf::Color(10,110,191));
    window.draw(player.draw_this());
    window.draw(map.get_environments().front()->draw_this());
    window.draw(map.get_environments().back()->draw_this());
    window.display();

    //cout << "Graphics updated" << endl;
}

void Game::handle_player_input(sf::Keyboard::Key key, bool is_pressed)
{
    if (key == sf::Keyboard::A)
	move_left = is_pressed;
    else if (key == sf::Keyboard::D)
	move_right = is_pressed;
    else if (key == sf::Keyboard::Space)
    {
	if(is_pressed)
	    player.jump();
    }
}
