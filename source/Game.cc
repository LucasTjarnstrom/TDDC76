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
#include "Coin.h"
#include "Weapon.h"
#include "Collision.h"
#include "High_Score_List.h"
#include "Enemy.h"
#include "Goal.h"

using namespace std;

Game::Game()
    : window(sf::VideoMode(1280, 720), "Help Alucard rescue the cat in this gothic adventure"),
      player{Player(650,430,0,0,"resources/player.png")}

{
    player.set_vitality(10);
    player.set_current_health(player.get_vitality());
    player.set_strength(2);

    window.setFramerateLimit(60); // FPS set to 60

    // Font
    old_london.loadFromFile("resources/OldLondon.ttf");

    //Background
    bgtexture.loadFromFile("resources/background.png");
    bgsprite.setTexture(bgtexture);
    
    // Setting up GUI for displaying player's health
    player_health.setFont(old_london);
    player_health.setCharacterSize(30);
    player_health.setColor(sf::Color(249, 72, 59));

    // Setting up GUI for displaying player's strength + weapon damage
    player_attack.setFont(old_london);
    player_attack.setCharacterSize(30);
    player_attack.setStyle(sf::Text::Bold);
    player_attack.setColor(sf::Color(249, 72, 59));
    player_attack.setPosition(20,0);

    // Setting up GUI for displaying player's score
    player_score.setFont(old_london);
    player_score.setCharacterSize(30);
    player_score.setStyle(sf::Text::Bold);
    player_score.setColor(sf::Color(249, 72, 59));
    player_score.setPosition(20,30);

    enter_your_name.setFont(old_london);
    enter_your_name.setCharacterSize(30);
    enter_your_name.setStyle(sf::Text::Bold);
    enter_your_name.setColor(sf::Color(255, 255, 255));
    enter_your_name.setPosition(500,400);
    enter_your_name.setString("Enter your name:");

    name_entry.setFont(old_london);
    name_entry.setCharacterSize(30);
    name_entry.setStyle(sf::Text::Bold);
    name_entry.setColor(sf::Color(255, 255, 255));
    name_entry.setPosition(500,440);
    
    // Environment objects
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

    map.create_environment("goal",710,600,0,0);

    map.create_environment("floor_filler",10,450,0,0);
    map.create_environment("floor_filler",50,450,0,0);
    map.create_environment("floor_filler",90,450,0,0);
    map.create_environment("floor_filler",130,450,0,0);
    map.create_environment("floor_filler",150,450,0,0);
    map.create_environment("floor_filler",10,490,0,0);
    map.create_environment("floor_filler",50,490,0,0);
    map.create_environment("floor_filler",90,490,0,0);
    map.create_environment("floor_filler",130,490,0,0);
    map.create_environment("floor_filler",150,490,0,0);
    map.create_environment("floor_filler",10,500,0,0);
    map.create_environment("floor_filler",50,500,0,0);
    map.create_environment("floor_filler",90,500,0,0);
    map.create_environment("floor_filler",130,500,0,0);
    map.create_environment("floor_filler",150,500,0,0);

    map.create_environment("floor_filler",610,625,0,0);
    map.create_environment("floor_filler",650,625,0,0);
    map.create_environment("floor_filler",610,665,0,0);
    map.create_environment("floor_filler",650,665,0,0);
    map.create_environment("floor_filler",610,705,0,0);
    map.create_environment("floor_filler",650,705,0,0);

    map.create_environment("coin", 350,330,0,0);
    map.create_environment("coin", 450,330,0,0);
    map.create_environment("coin", 400,330,0,0);
    map.create_environment("coin", 950,230,0,0);
    map.create_environment("coin", 790,328,0,0);

    map.create_environment("weapon",150,640,0,0);
    map.create_environment("weapon",2000,0,0,0);

    // Enemy objects
    create_enemy("ghoul",200,600,0,0);
    create_enemy("ghoul",850,600,0,0);
    create_enemy("ghoul",1100,600,0,0);

    // for testing
    //enemies.front()->set_current_health(1);
    //enemies.back()->set_current_health(1);
    health_text.setCharacterSize(30);
    health_text.setStyle(sf::Text::Bold);
    health_text.setColor(sf::Color(255, 255, 255));
    health_text.setString("hello");
    health_text.setFont(old_london);

}

// borde uttökas så även vitality och current_health sätts 
void Game::create_enemy(std::string type, int xp, int yp, int xs, int ys)
{
  if (type == "ghoul")
    {
      unique_ptr<Enemy> temp = make_unique<Enemy>(xp,yp,xs,ys,"resources/enemy.png");
      temp->set_font(old_london); // enemy's health_text gets its font set
      temp->set_vitality(4); // a Ghoul has 4 vitality
      temp->set_current_health(temp->get_vitality()); // a Ghoul has max health when spawned
      temp->set_score(200); // a Ghoul has 200 score
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
	    }
	    else
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
    if(clock.getElapsedTime().asSeconds() >= 1)
    {
	// if(player.attack_counter <= 0)
	//     player.attack_counter++;
	player.attack_counter = 1; // kolla på detta
	    
	clock.restart();
    }
    
    if(move_left)
	movement = "left";
    else if(move_right)
	movement = "right";
    else
      movement = "nothing";
  
    player.move(movement);

    // check if any enemies have died and moves enemies
    if(!enemies.empty())
      {
	auto it = enemies.begin();
	while(it != enemies.end()){
	  if ((*it)->get_facing_right())
	    {
	      (*it) -> move("right");
	    }
	  else
	    {
	      (*it) -> move("left");
	    }
	  if((*it)->get_current_health() <= 0){
	      player.set_score(player.get_score() + (*it)->get_score());
	      it = enemies.erase(it);
	  }else
	    it++;
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
	  else if (dynamic_cast<Coin*> ((*it).get()) != nullptr) // checks if player is colliding with a Coin
	  {
	      player.is_colliding("coin");
	      it = map.get_environments().erase(it);
	      
	  }
	  else if (dynamic_cast<Goal*> ((*it).get()) != nullptr) //Checks if player is colliding with a Goal
	    {
	      game_won = true;
	    }
	  else if (dynamic_cast<Weapon*> ((*it).get()) != nullptr) // checks if player is colliding with a Coin
	  {
	      player.set_weapon_damage(player.get_weapon_damage() + 2);
//player.is_colliding("coin");
	      it = map.get_environments().erase(it);
	      
	  }
	  else
	  {
	      //----------Colliding with a Floor----------
	      player.is_colliding("floor");	      
	    }
	}
    }

  //----------ENEMY COLLISION WITH ENVIRONMENT----------
  if(!enemies.empty()){
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
  }
  
  // ------ Collision with Player's Attack and enemies -------
  if(attacking)
  {
      if(player.attack_counter > 0)
      {
	  if(!enemies.empty())
	  {
	      for (auto enemyit = enemies.begin(); enemyit != enemies.end(); enemyit++)
	      {
		  if (Collision::BoundingBoxTest(player.attack(), (*enemyit)->draw_this()))
		  {
		      (*enemyit)->set_current_health(enemies.front()->get_current_health() - 
						     player.get_strength() - player.get_weapon_damage());
		  }
	      }
	  } 
      }
  }
  
  
  window.clear();
  window.draw(bgsprite);
  
  if (!game_won)
  window.draw(player.draw_this());
  
  if(!enemies.empty()) // Draws enemies and their health_text
  {
      for (auto it = enemies.begin(); it != enemies.end(); it++)
      {
	  window.draw((*it) -> draw_this());
	  //window.draw((*it) -> health_to_text()); // ger seg. fault
      }
  }
    
  for (auto it = map.get_environments().begin(); it != map.get_environments().end(); it++)
    {
      window.draw((*it) -> draw_this());
    }
  
  if(attacking)
    {
      if(player.attack_counter > 0)
	{
	  window.draw(player.attack());
	  player.attack_counter--; 
	}
    }
  if (!game_won)
    window.draw(draw_player_health());
  else
    {
      window.draw(enter_your_name);
      window.draw(name_entry);
    }
  window.draw(draw_player_attack());
  window.draw(draw_player_score());
  //window.draw(health_text);
  //window.draw(enemies.front()->health_text);
  window.display();
  //cout << "Graphics updated" << endl;
}

void Game::handle_player_input(sf::Keyboard::Key key, bool is_pressed)
{
  if (!game_won)
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
      else if (key == sf::Keyboard::W)
	attacking = is_pressed;
    }
  else
    {
      if(is_pressed)
	{
	  stringstream ss;
	  string temp = name_entry.getString().toAnsiString();
	  if (key == sf::Keyboard::A) //friday afternoon code, pls don't judge
	    temp.append("A");
	  else if (key == sf::Keyboard::B)
	    temp.append("B");
	  else if (key == sf::Keyboard::C)
	    temp.append("C");
	  else if (key == sf::Keyboard::D)
	    temp.append("D");
	  else if (key == sf::Keyboard::E)
	    temp.append("E");
	  else if (key == sf::Keyboard::F)
	    temp.append("F");
	  else if (key == sf::Keyboard::G)
	    temp.append("G");
	  else if (key == sf::Keyboard::H)
	    temp.append("H");
	  else if (key == sf::Keyboard::I)
	    temp.append("I");
	  else if (key == sf::Keyboard::J)
	    temp.append("J");
	  else if (key == sf::Keyboard::K)
	    temp.append("K");
	  else if (key == sf::Keyboard::L)
	    temp.append("L");
	  else if (key == sf::Keyboard::M)
	    temp.append("M");
	  else if (key == sf::Keyboard::N)
	    temp.append("N");
	  else if (key == sf::Keyboard::O)
	    temp.append("O");
	  else if (key == sf::Keyboard::P)
	    temp.append("P");
	  else if (key == sf::Keyboard::Q)
	    temp.append("Q");
	  else if (key == sf::Keyboard::R)
	    temp.append("R");
	  else if (key == sf::Keyboard::S)
	    temp.append("S");
	  else if (key == sf::Keyboard::T)
	    temp.append("T");
	  else if (key == sf::Keyboard::U)
	    temp.append("U");
	  else if (key == sf::Keyboard::V)
	    temp.append("V");
	  else if (key == sf::Keyboard::W)
	    temp.append("W");
	  else if (key == sf::Keyboard::X)
	    temp.append("X");
	  else if (key == sf::Keyboard::Y)
	    temp.append("Y");
	  else if (key == sf::Keyboard::Z)
	    temp.append("Z");

	  name_entry.setString(temp);
	}
    }
}

sf::Text Game::draw_player_health()
{
    stringstream ss;
    ss << player.get_current_health() << "/" << player.get_vitality();
    player_health.setString(ss.str());
    player_health.setPosition(player.get_x_pos() - 28, player.get_y_pos() - 40);

    return player_health;
}

sf::Text Game::draw_player_attack()
{
    stringstream ss;
    ss << "Attack: " << player.get_strength() << " + " << player.get_weapon_damage();
    player_attack.setString(ss.str());

    return player_attack;
}

sf::Text Game::draw_player_score()
{
    stringstream ss;
    ss << "Score: " << player.get_score() << endl; 
    player_score.setString(ss.str());

    return player_score;
}
