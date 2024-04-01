// duckBuster.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    bool isLooping(true);
    bool endMessage(false);

    while (isLooping) {
        RenderWindow window(VideoMode(800, 600), "Duck Hunter");
        World world(Vector2f(0, 0));
        int score(0);
        int arrows(5);

        SoundBuffer popBuffer;
        if (!popBuffer.loadFromFile("duckdestroy.ogg")) {
            cout << "Could not load duckdestroy.ogg" << endl;
            exit(5);
        }
        Sound popSound;
        popSound.setBuffer(popBuffer);

        Music music;
        if (!music.openFromFile("circus.ogg")) {
            cout << "Failed to load circus.ogg ";
            exit(6);
        }
        music.play();

        Font fnt;
        if (!fnt.loadFromFile("arial.ttf")) {
            cout << "Could not load font." << endl;
            exit(3);
        }

        PhysicsSprite& crossBow = *new PhysicsSprite();
        Texture cbowTex;
        LoadTex(cbowTex, "images/crossbow.png");
        crossBow.setTexture(cbowTex);
        Vector2f szBow = crossBow.getSize();
        crossBow.setCenter(Vector2f(400, 600 - (szBow.y / 2)));

        PhysicsSprite arrow;
        Texture arrowTex;
        LoadTex(arrowTex, "images/arrow.png");
        arrow.setTexture(arrowTex);
        bool drawingArrow(false);

        PhysicsRectangle right;
        right.setSize(Vector2f(10, 600));
        right.setCenter(Vector2f(795, 300));
        right.setStatic(true);
        world.AddPhysicsBody(right);

        Texture duckTex;
        LoadTex(duckTex, "images/duck.png");
        PhysicsShapeList<PhysicsSprite> ducks;

        PhysicsRectangle top;
        top.setSize(Vector2f(800, 10));
        top.setCenter(Vector2f(400, 5));
        top.setStatic(true);
        world.AddPhysicsBody(top);

        top.onCollision = [&drawingArrow, &world, &arrow]
        (PhysicsBodyCollisionResult result) {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
            };

        Clock clock;
        Time lastTime(clock.getElapsedTime());
        Time currentTime(lastTime);

        long duckMS(2000);
        while ((arrows > 0) || drawingArrow) {
            currentTime = clock.getElapsedTime();
            Time deltaTime = currentTime - lastTime;
            long deltaMS = deltaTime.asMilliseconds();
            if (deltaMS > 9) {
                lastTime = currentTime;
                world.UpdatePhysics(deltaMS);
                duckMS = duckMS + (deltaMS);
                if (Keyboard::isKeyPressed(Keyboard::Space) &&
                    !drawingArrow) {
                    drawingArrow = true;
                    arrows = arrows - 1;
                    arrow.setCenter(crossBow.getCenter());
                    arrow.setVelocity(Vector2f(0, -1));
                    world.AddPhysicsBody(arrow);
                }

                window.clear();
                if (drawingArrow) {
                    window.draw(arrow);
                }

                ducks.DoRemovals();
                for (PhysicsShape& duck : ducks) {
                    window.draw((PhysicsSprite&)duck);
                }

                window.draw(crossBow);
                Text scoreText;
                scoreText.setString(to_string(score));
                scoreText.setFont(fnt);
                scoreText.setPosition(Vector2f(GetTextSize(scoreText).x, 580 - GetTextSize(scoreText).y));
                window.draw(scoreText);
                Text arrowCountText;
                arrowCountText.setString(to_string(arrows));
                arrowCountText.setFont(fnt);
                arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 580 - GetTextSize(arrowCountText).y));
                window.draw(arrowCountText);
                //world.VisualizeAllBounds(window);

                window.display();

            }

            if (duckMS > 1000) {
                duckMS = 0;

                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTex);
                Vector2f sz = duck.getSize();
                duck.setCenter(Vector2f(0, 20 + (sz.y / 2)));
                duck.setVelocity(Vector2f(0.5, 0));
                world.AddPhysicsBody(duck);
                duck.onCollision =
                    [&drawingArrow, &world, &arrow, &right, &duck, &ducks, &score, &popSound]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        popSound.play();
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    if (result.object2 == right) {
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                    }
                    };
            }
        }
        window.display();
        Text gameOverText;
        gameOverText.setString("GAME OVER");
        gameOverText.setFont(fnt);
        Vector2f szText = GetTextSize(gameOverText);
        gameOverText.setPosition(400 - (szText.x / 2), 300 - (szText.y / 2));
        window.draw(gameOverText);
        window.display();
        endMessage = true;
        while (endMessage) {
            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                endMessage = false;
            }
        }
    }
}