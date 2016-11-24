// pong_sfml.cpp : Defines the entry point for the console application.
//THIS PONG COMES FROM SFML_EXEMPLES

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "../../AI/Lia/Lia v1/Lia v1/IA.h"


// Define some constants
const float pi = 3.14159f;
const int gameWidth = 800;
const int gameHeight = 600;
sf::Vector2f paddleSize(25, 100);
float ballRadius = 10.f;

sf::RectangleShape leftPaddle;
sf::RectangleShape rightPaddle;

const float paddleSpeed = 400.f;
float rightPaddleSpeed = 0.f;
const float ballSpeed = 400.f;

float deltaTime;

sf::Clock AITimer;
const sf::Time AITime = sf::seconds(0.1f);
float ballAngle = 0.f; // to be changed later
sf::CircleShape ball;

bool isPlaying = false;

int ballPosY; //coordonne relative au player .. Y serait le droite / gauche .. et X la distance
int ballPosX;
bool firstGen = true;

void moveUp(int work);
void moveDown(int work);
void restart(sf::Clock *clock, sf::Clock *timer);

int main()
{


	std::string neuronBase = "0:0!0|0:0!0|1:0!0|2:10!0|2:10!0|";
	IA Lia(neuronBase);
	Lia.addOutput(&moveUp);
	Lia.addOutput(&moveDown);
	Lia.addInput(&ballPosY, 0);
	Lia.addInput(&ballPosX, 1);

	vector<IA> genom;
	vector<int> score;
	int scoreMax = 1000000;
	int scoreTemp1;
	int scoreTemp1Case;
	int scoreTemp2;
	int scoreTemp2Case;
	bool wow = false;

	IA *tempIa = 0;

	vector<IA> generations;
	generations.push_back(Lia);




	sf::Clock clock;
	sf::Clock timer;

	std::srand(static_cast<unsigned int>(std::time(NULL)));

	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "IA TEST",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	leftPaddle.setOutlineThickness(3);
	leftPaddle.setOutlineColor(sf::Color::Black);
	leftPaddle.setFillColor(sf::Color(100, 100, 200));
	leftPaddle.setOrigin(paddleSize / 2.f);

	rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	rightPaddle.setOutlineThickness(3);
	rightPaddle.setOutlineColor(sf::Color::Black);
	rightPaddle.setFillColor(sf::Color(200, 100, 100));
	rightPaddle.setOrigin(paddleSize / 2.f);

	ball.setRadius(ballRadius - 3);
	ball.setOutlineThickness(3);
	ball.setOutlineColor(sf::Color::Black);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ballRadius / 2, ballRadius / 2);

	restart(&clock, &timer);
	while (window.isOpen())
	{
		//sf::sleep(sf::milliseconds(10)); //NE SERT A RIEN PUT***!
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				restart(&clock, &timer);
			}
		}

		if (isPlaying)
		{

			if (firstGen)
			{
				tempIa = new IA(generations[generations.size() - 1].returnADN());
				tempIa->mutate();

				genom.push_back(IA(tempIa->returnADN()));
				genom[genom.size() - 1].addOutput(&moveUp);
				genom[genom.size() - 1].addOutput(&moveDown);
				genom[genom.size() - 1].addInput(&ballPosY, 0);
				genom[genom.size() - 1].addInput(&ballPosX, 1);
				score.push_back(0);
				delete tempIa;
				tempIa = 0;
				wow = false;
				firstGen = false;
			}

			deltaTime = clock.restart().asSeconds();
			// Move the player's paddle
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				moveUp(0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				moveDown(0);
			}*/
			genom[genom.size() - 1].update();
			cout << "ADN: " << genom[genom.size() - 1].returnADN() << endl;

			// Move the computer's paddle
			if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
				((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
			{
				rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
			}

			// Update the computer's paddle direction according to the ball position
			if (AITimer.getElapsedTime() > AITime)
			{
				AITimer.restart();
				if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
					rightPaddleSpeed = paddleSpeed;
				else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
					rightPaddleSpeed = -paddleSpeed;
				else
					rightPaddleSpeed = 0.f;
			}

			// Move the ball
			float factor = ballSpeed * deltaTime;
			ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

			// Check collisions between the ball and the screen
			if (ball.getPosition().x - ballRadius < 0.f)
			{
				isPlaying = false;
				//pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}
			if (ball.getPosition().x + ballRadius > gameWidth)
			{
				isPlaying = false;
				//pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
			}
			if (ball.getPosition().y - ballRadius < 0.f)
			{
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
			}
			if (ball.getPosition().y + ballRadius > gameHeight)
			{
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
			}

			// Check the collisions between the ball and the paddles
			// Left Paddle
			if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
				ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > leftPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
			}

			// Right Paddle
			if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
				ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > rightPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
			}
		}

		window.clear(sf::Color(50, 200, 50));

		if (isPlaying)
		{
			window.draw(leftPaddle);
			window.draw(rightPaddle);
			window.draw(ball);
		}
		else //PERDU! / FIN DE GAME
		{
			score[score.size() - 1] = timer.getElapsedTime().asSeconds();
			if (genom.size() > 11)
			{
				scoreTemp1 = -1; scoreTemp2 = -1;
				for (unsigned int i(0); i < score.size(); i++)
				{
					if (scoreTemp1 < score[i])
					{
						scoreTemp1 = score[i];
						scoreTemp1Case = i;
					}
				}
				if (scoreTemp1 > scoreMax)
				{
					Beep(1000, 1000);
					cout << "wow; score: " << scoreTemp1 << endl << "ADN: " << genom[scoreTemp1Case].returnADN() << endl << endl;
					wow = true;
				}
				for (unsigned int i(0); i < score.size(); i++)
				{
					if (scoreTemp2 < score[i] && scoreTemp1Case != i)
					{
						scoreTemp2 = score[i];
						scoreTemp2Case = i;
					}
				}
				if (scoreTemp2 > scoreMax)
				{
					Beep(1000, 1000);
					cout << "wow; score: " << scoreTemp2 << endl << "ADN: " << genom[scoreTemp2Case].returnADN() << endl << endl;
					wow = true;
				}

				//fusion
				//cout << "fusion!" << endl;
				generations.clear();
				generations.push_back(IA(Lia.fusion(genom[scoreTemp1Case].returnADN(), genom[scoreTemp2Case].returnADN())));
				genom.clear();
				score.clear();

				if (wow)
				{
					cout << "fusion ADN: " << generations[generations.size() - 1].returnADN() << endl;
					cout << endl << "PAUSE!" << endl << endl;
					//break;
				}
				else{

				}
					//break;
				restart(&clock, &timer);
			}
			else
			{
				tempIa = new IA(generations[generations.size() - 1].returnADN());
				tempIa->mutate();

				genom.push_back(IA(tempIa->returnADN()));
				genom[genom.size() - 1].addOutput(&moveUp);
				genom[genom.size() - 1].addOutput(&moveDown);
				genom[genom.size() - 1].addInput(&ballPosY, 0);
				genom[genom.size() - 1].addInput(&ballPosX, 1);
				score.push_back(0);
				delete tempIa;
				tempIa = 0;

				wow = false;
			}

		}

		window.display();
	}

	return 0;
}

void moveUp(int work)
{
	if (work > 0) {
		if (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f)
		{
			leftPaddle.move(0.f, -paddleSpeed * deltaTime);
		}
	}
}

void moveDown(int work)
{
	if (work > 0)
	{
		if (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)
		{
			leftPaddle.move(0.f, paddleSpeed * deltaTime);
		}
	}
}

void restart(sf::Clock *clock, sf::Clock *timer)
{
	// (re)start the game
	isPlaying = true;
	clock->restart();
	timer->restart();
	firstGen = true;

	// Reset the position of the paddles and ball
	leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
	rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
	ball.setPosition(gameWidth / 2, gameHeight / 2);

	// Reset the ball angle
	do
	{
		// Make sure the ball initial angle is not too much vertical
		ballAngle = (std::rand() % 360) * 2 * pi / 360;
	} while (std::abs(std::cos(ballAngle)) < 0.7f);
}