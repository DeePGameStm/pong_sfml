// pong_sfml.cpp : Defines the entry point for the console application.
//THIS PONG COMES FROM SFML_EXEMPLES
//BUT YOLO!

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

const float paddleSpeed = 8.f;
float rightPaddleSpeed = 0.f;
const float ballSpeed = 6.f;

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

bool vsync = true;

int main()
{
	int nbGeneration = 0;
	std::cout << "neuronBase (enter = default): ";
	std::string neuronBase;
	getline(cin, neuronBase);
	if(neuronBase.size() < 3)
		neuronBase = "0:0!0|0:0!0|1:0!0|2:10!0|2:10!0|"; //0:0!0|0:0!0|1:0!0|2:10!0|2:10!0|
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

	bool AiWin;
	bool gameStop = false;
	int genomMax = 12;


	sf::Clock clock;
	sf::Clock timer;

	std::srand(static_cast<unsigned int>(std::time(NULL)));
	bool botControlled = false;

	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "IA TEST",
		sf::Style::Titlebar | sf::Style::Close);

	window.setVerticalSyncEnabled(vsync);

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

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::V))
			{
				if (vsync)
					vsync = false;
				else
					vsync = true;
				window.setVerticalSyncEnabled(vsync);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::C))
			{
				if (botControlled)
					botControlled = false;
				else
					botControlled = true;
			}
		}

		if (isPlaying)
		{

			if (firstGen)
			{
				cout << "NEWGEN" << endl;
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

			//deltaTime = clock.restart().asSeconds();
			// Move the player's paddle
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				moveUp(0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				moveDown(0);
			}*/
			ballPosY = ball.getPosition().y - leftPaddle.getPosition().y;
			ballPosX = ball.getPosition().x;
			genom[genom.size() - 1].update();
			//cout << "ADN: " << genom[genom.size() - 1].returnADN() << endl;

			if (!botControlled) {
				// Move the computer's paddle
				if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
					((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
				{
					rightPaddle.move(0.f, rightPaddleSpeed);
				}

				// Update the computer's paddle direction according to the ball position
				if (AITimer.getElapsedTime() > AITime || true)
				{
					AITimer.restart();
					if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
						rightPaddleSpeed = paddleSpeed;
					else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
						rightPaddleSpeed = -paddleSpeed;
					else
						rightPaddleSpeed = 0.f;
				}
			}
			else
			{

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					if (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)
					{
						rightPaddle.move(0.f, -paddleSpeed);
					}
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					if (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)
					{
						rightPaddle.move(0.f, paddleSpeed);
					}
				}
			}

			// Move the ball
			float factor = ballSpeed;
			ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

			// Check collisions between the ball and the screen
			if (ball.getPosition().x - ballRadius < 0.f)
			{
				isPlaying = false;
				AiWin = false;
				gameStop = true;
				//pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}
			if (ball.getPosition().x + ballRadius > gameWidth)
			{
				isPlaying = false;
				AiWin = true;
				gameStop = true;
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
				score[score.size() - 1]++;
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

			if (gameStop)
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
				gameStop = false;
			}
		}

		if(vsync)
			window.clear(sf::Color(50, 200, 50));

		if (isPlaying)
		{
			if (timer.getElapsedTime().asSeconds() > 120)
			{
				cout << "----------------" << endl << "ADN: " << genom[genom.size() - 1].returnADN() << endl << "nbGeneration: " << nbGeneration << endl << "----------------" << endl;
				system("PAUSE");
				return 0;
			}

			if (vsync)
			{
				window.draw(leftPaddle);
				window.draw(rightPaddle);
				window.draw(ball);
			}
		}
		else //PERDU! / FIN DE GAME
		{
			//score[score.size() - 1] = timer.getElapsedTime().asSeconds();
			if(AiWin)
				score[score.size() - 1] += 999;

			if (genom.size() > genomMax - 1)
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

				/*if (scoreTemp1Case == 0)
					scoreTemp1Case = (rand() % (genomMax - 2)) + 1;
				if (scoreTemp2Case == 0)
					scoreTemp2Case = (rand() % (genomMax - 2)) + 1;*/

				cout << "Best ADN1: " << genom[scoreTemp1Case].returnADN() << endl << "Best ADN2: " << genom[scoreTemp2Case].returnADN() << endl << endl;
				cout << "Best score1: " << scoreTemp1 << endl << "Best score2: " << scoreTemp2 << endl;

				//fusion
				//cout << "fusion!" << endl;
				generations.clear();
				generations.push_back(IA(Lia.fusion(genom[scoreTemp1Case].returnADN(), genom[scoreTemp2Case].returnADN())));
				nbGeneration++;
				genom.clear();
				score.clear();
				cout << "result: " << generations[generations.size() - 1].returnADN() << endl << endl;

				if (wow)
				{
					cout << "fusion ADN: " << generations[generations.size() - 1].returnADN() << endl;
					cout << endl << "PAUSE!" << endl << endl;
					//break;
				}
				else{

				}
					//break;
				firstGen = true;
				restart(&clock, &timer);
			}
			else
			{
				restart(&clock, &timer);
			}

		}

		if(vsync)
			window.display();
	}

	return 0;
}

void moveUp(int work)
{
	if (work > 0) {
		if (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f)
		{
			leftPaddle.move(0.f, -paddleSpeed);
		}
	}
}

void moveDown(int work)
{
	if (work > 0)
	{
		if (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)
		{
			leftPaddle.move(0.f, paddleSpeed);
		}
	}
}

void restart(sf::Clock *clock, sf::Clock *timer)
{
	// (re)start the game
	isPlaying = true;
	clock->restart();
	timer->restart();

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
	//if((rand() % 2) == 1)
		ballAngle = 147 * 2 * pi / 360;
	//else
		//ballAngle = 215 * 2 * pi / 360;
}
