/*
 * Game.cc
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <sstream>
#include <vector>

#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Floor.h"
#include "Wall.h"
#include "Ceiling.h"
#include "Collision.h"
#include "High_Score_List.h"
#include "Enemy.h"

using namespace std;

Game::Game()
    : window(sf::VideoMode(1280, 720), "Help Alucard rescue the cat in this gothic adventure"),
      player{Player(200,550,0,0,"resources/player.png")}

{
    player.set_vitality(10);
    player.set_current_health(player.get_vitality());
    player.set_strength(1);

    window.setFramerateLimit(60); // FPS set to 60

    // Font
    arial.loadFromFile("resources/arial.ttf");

    //Background
    bgtexture.loadFromFile("resources/background.png");
    bgsprite.setTexture(bgtexture);
    
    // Setting up GUI for displaying player's health
    player_health.setFont(arial);
    player_health.setCharacterSize(30);
    player_health.setStyle(sf::Text::Bold);
    player_health.setColor(sf::Color(255, 255, 255));

    // Setting up GUI for displaying player's strength + weapon damage
    player_attack.setFont(arial);
    player_attack.setCharacterSize(30);
    player_attack.setStyle(sf::Text::Bold);
    player_attack.setColor(sf::Color(255, 255, 255));
    player_attack.setPosition(960,0);

    // Setting up GUI for displaying player's score
    player_score.setFont(arial);
    player_score.setCharacterSize(30);
    player_score.setStyle(sf::Text::Bold);
    player_score.setColor(sf::Color(255, 255, 255));
    player_score.setPosition(960,40);
    
    // Setting up testing for enemy hp
    enemy_health.setFont(arial);
    enemy_health.setCharacterSize(30);
    enemy_health.setStyle(sf::Text::Bold);
    enemy_health.setColor(sf::Color(255, 255, 255));
    enemy_health.setPosition(400,400);
    
    map.create_environment("floor_1280px",0,690,0,0);
    map.create_environment("wall",0,190,0,0);
    map.create_environment("wall",0,0,0,0);
    map.create_environment("wall",1270,190,0,0);
    map.create_environment("wall",1270,0,0,0);

    map.create_environment("wall_100px",600,620,0,0);
    map.create_environment("floor_100px",600,615,0,0);
    map.create_environment("wall_100px",690,620,0,0);

    map.create_environment("ceiling",0,550,0,0);
    map.create_environment("floor",0,540,0,0);
    map.create_environment("wall_20px",490,540,0,0);

    map.create_environment("floor",300,360,0,0);
    map.create_environment("ceiling",300,370,0,0);
    map.create_environment("wall_20px",290,360,0,0);
    map.create_environment("wall_20px",800,360,0,0);

    map.create_environment("wall_100px",190,450,0,0);
    map.create_environment("floor_100px",10,440,0,0);
    map.create_environment("floor_100px",100,440,0,0);

    map.create_environment("wall",690,270,0,0);
    map.create_environment("wall",700,270,0,0);
    map.create_environment("floor",690,260,0,0);
    map.create_environment("ceiling",690,270,0,0);
    map.create_environment("wall_20px",1190,260,0,0);
	
    create_enemy("ghoul",200,600,0,0);
    create_enemy("ghoul",850,600,0,0);
    create_enemy("ghoul",1100,600,0,0);

    enemies.front()->set_vitality(10);
    enemies.front()->set_current_health(10);
    
}

void Game::create_enemy(std::string type, int xp, int yp, int xs, int ys)
{
  if (type == "ghoul")
    {
      unique_ptr<Enemy> temp = make_unique<Enemy>(xp,yp,xs,ys,"resources/enemy.png");
      add_enemy(move(temp));
    }
  else
    {
      throw logic_error("trying to create Enemy of invalid type");
    }
}

vector<unique_ptr<Enemy>>& Game::get_enemies()
{
  return enemies;
}

void Game::set_enemies(vector<unique_ptr<Enemy>>& other)
{
  enemies = move(other);
}

void Game::add_enemy(unique_ptr<Enemy> e)
{
  enemies.push_back(move(e));
}

void Game::run(string user_choice)
{
    if ( user_choice == "Start game" )
    {
	clock.restart(); // restart clock
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
    //cout << clock.getElapsedTime().asSeconds() << endl;
    if(clock.getElapsedTime().asSeconds() >= 1)
    {
	if(player.attack_counter <= 0)
	    player.attack_counter++;
	    
	clock.restart();
    }
    
    if(move_left)
	movement = "left";
    else if(move_right)
	movement = "right";
    else
      movement = "nothing";
  
    player.move(movement);

    for (auto it = enemies.begin(); it != enemies.end(); it++)
      {
	if ((*it)->get_facing_right())
	  {
	    (*it) -> move("right");
	  }
	else
	  {
	    (*it) -> move("left");
	  }
      }
}

void Game::render()
{
  //----------PLAYER COLLISION WITH ENVIRONMENT----------
  for (auto it = map.get_environments().begin(); it != map.get_environments().end(); it++)
    {
      if (Collision::BoundingBoxTest(player.draw_this(), (*it)->draw_this())) //Checks if player is colliding with anything
	{
	  if (dynamic_cast<Wall*> ((*it).get()) != nullptr) // Checks if player is colliding with a Wall
	    {
	      for (auto it2 = map.get_environments().begin(); it2 != map.get_environments().end(); it2++)
		{
		  if (Collision::BoundingBoxTest(player.draw_this(), (*it2)->draw_this())) //Checks if player is colliding with another object
		    {
		      if (dynamic_cast<Floor*> ((*it2).get()) != nullptr) // Checks if the other object is a Floor
			{
			  //----------Colliding with both a Floor and a Wall----------
			  player.is_colliding("floor_wall");
			}
		      else
			{
			  //----------Colliding with a Wall----------
			  player.is_colliding("wall");	  
			}
		    }
		}
	    }
	  else if (dynamic_cast<Ceiling*> ((*it).get()) != nullptr) //Checks if player is colliding with a Ceiling
	    {
	      player.is_colliding("ceiling");
	    }
	  else
	    {
	      //----------Colliding with a Floor----------
	      player.is_colliding("floor");	      
	    }
	}
    }

  //----------ENEMY COLLISION WITH ENVIRONMENT----------
  for (auto enemyit = enemies.begin(); enemyit != enemies.end(); enemyit++)
    {
      for (auto it = map.get_environments().begin(); it != map.get_environments().end(); it++)
	{
	  if (Collision::BoundingBoxTest((*enemyit)->draw_this(), (*it)->draw_this())) //Checks if the enemy is colliding with anything
	    {
	      if (dynamic_cast<Wall*> ((*it).get()) != nullptr) // Checks if the enemy is colliding with a Wall
		{
		  for (auto it2 = map.get_environments().begin(); it2 != map.get_environments().end(); it2++)
		    {
		      if (Collision::BoundingBoxTest((*enemyit)->draw_this(), (*it2)->draw_this())) //Checks if the enemy is colliding with another object
			{
			  if (dynamic_cast<Floor*> ((*it2).get()) != nullptr) // Checks if the other object is a Floor
			    {
			      //----------Colliding with both a Floor and a Wall----------
				(*enemyit)->is_colliding("wall");
			    }
			  else
			    {
			      //----------Colliding with a Wall----------
				(*enemyit)->is_colliding("wall");
			    }
			}
		    }
		}
	      else
		{
		  //----------Colliding with a Floor----------
		    (*enemyit)->is_colliding("floor");
		}
	    }
	}
    }
  
  if(attacking) //fulhax extravaganza (kollar om fiende kolliderar med players attack)
  {
      if(player.attack_counter > 0)
      {
	  if (Collision::BoundingBoxTest(player.attack(), enemies.front()->draw_this()))
	  {
	      //enemies.front()->set_y_velocity(enemies.front()->get_y_velocity() + -0.5);
	      enemies.front()->set_current_health(enemies.front()->get_current_health() - 
						  player.get_strength());
	  }
	  player.attack_counter--;
	    
      }
 }
  
  window.clear();

    window.draw(bgsprite);
  
    window.draw(player.draw_this());
    for (auto it = enemies.begin(); it != enemies.end(); it++)
      {
	window.draw((*it) -> draw_this());
      }
    for (auto it = map.get_environments().begin(); it != map.get_environments().end(); it++)
    {
	window.draw((*it) -> draw_this());
    }

    if(attacking)
	if(player.attack_counter > 0)
	    window.draw(player.attack());
    window.draw(draw_player_health());
    window.draw(draw_player_attack());
    window.draw(draw_enemy_health());
    window.draw(draw_player_score());
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
    } else if (key == sf::Keyboard::W)
    {
	// 	cout << clock.getElapsedTime().asSeconds() << endl;
	// 	if(is_pressed){
	// 	    cout << "Magic Missile!" << endl; // player.
	// 	    if(clock.getElapsedTime().asSeconds() >= 1.0)
	// 	    {
	// 		cout << "attacking" << endl;
	// 	        attacking = true;
	// 		clock.restart();
	// 	    } else
	// 		attacking = false;
	    
	// 	} else
	// 	    attacking = false;
	// }
	//if(is_pressed){
	//    cout << "Magic Missile!" << endl;
	//}
	attacking = is_pressed;
    }
}

sf::Text Game::draw_player_health()
{
    stringstream ss;
    ss << player.get_current_health() << "/" << player.get_vitality();
    player_health.setString(ss.str());
    player_health.setPosition(player.get_x_pos() - 40, player.get_y_pos() - 40);

    return player_health;
}

sf::Text Game::draw_player_attack()
{
    stringstream ss;
    ss << "Your attack is: " << player.get_strength() << " + " << player.get_weapon_damage();
    player_attack.setString(ss.str());

    return player_attack;
}

sf::Text Game::draw_enemy_health()
{
    stringstream ss;
    ss << enemies.front()->get_current_health() << "/" << enemies.front()->get_vitality();
    enemy_health.setString(ss.str());
    enemy_health.setPosition(enemies.front()->get_x_pos() - 30, enemies.front()->get_y_pos() - 40);
    
    return enemy_health;

}

sf::Text Game::draw_player_score()
{
    stringstream ss;
    ss << "Your score is: " << player.get_score() << endl; 
    player_score.setString(ss.str());

    return player_score;
}
