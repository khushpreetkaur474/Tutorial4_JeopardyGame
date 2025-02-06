/*
 * Tutorial 4 Jeopardy Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "questions.h"
#include "players.h"
#include "jeopardy.h"

// Define constants
#define BUFFER_LEN 256
#define NUM_PLAYERS 4

// Function prototypes
void tokenize(char *input, char *tokenized_answer);
void show_results(player *players, int num_players);

int main(int argc, char *argv[])
{
    // Array of players
    player players[NUM_PLAYERS];

    // Input buffer
    char buffer[BUFFER_LEN] = {0};

    // Display game introduction and initialize questions
    initialize_game();

    // Prompt for players' names and initialize scores
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        printf("Enter the name of player %d: ", i + 1);
        fgets(players[i].name, MAX_LEN, stdin);
        players[i].name[strcspn(players[i].name, "\n")] = '\0'; // Remove newline
        players[i].score = 0;
    }

    // Game loop
    while (!all_questions_answered()) // Continue until all questions are answered
    {
        display_categories();

        // Get player name
        printf("Enter player name to choose a category: ");
        fgets(buffer, BUFFER_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (!player_exists(players, NUM_PLAYERS, buffer))
        {
            printf("Invalid player name. Try again.\n");
            continue;
        }

        char category[MAX_LEN];
        int value;

        // Get category and value
        printf("Enter category and value: ");
        if (scanf("%s %d", category, &value) != 2)
        {
            printf("Invalid input format.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }
        getchar(); // Consume leftover newline

        if (already_answered(category, value))
        {
            printf("Question already answered. Choose another.\n");
            continue;
        }

        display_question(category, value);

        // Get player answer
        printf("Enter your answer: ");
        fgets(buffer, BUFFER_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        char tokenized_answer[MAX_LEN] = "";
        tokenize(buffer, tokenized_answer);

        if (valid_answer(category, value, tokenized_answer))
        {
            printf("Correct answer!\n");
            update_score(players, NUM_PLAYERS, buffer, value);
        }
        else
        {
            printf("Incorrect! The correct answer was: %s\n", get_correct_answer(category, value));
        }

        // Mark question as answered
        mark_as_answered(category, value);
    }

    // Display final results
    show_results(players, NUM_PLAYERS);

    return EXIT_SUCCESS;
}

// Tokenizes the answer, extracting the relevant part
void tokenize(char *input, char *tokenized_answer)
{
    char *token = strtok(input, " ");
    if (token && (strcmp(token, "who") == 0 || strcmp(token, "what") == 0))
    {
        token = strtok(NULL, " ");
        if (token && strcmp(token, "is") == 0)
        {
            token = strtok(NULL, "");
            if (token)
            {
                strcpy(tokenized_answer, token);
            }
        }
    }
}

// Displays the final rankings
void show_results(player *players, int num_players)
{
    // Sort players by score in descending order
    for (int i = 0; i < num_players - 1; i++)
    {
        for (int j = i + 1; j < num_players; j++)
        {
            if (players[i].score < players[j].score)
            {
                player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    printf("\nFinal Results:\n");
    for (int i = 0; i < num_players; i++)
    {
        printf("%d. %s - %d points\n", i + 1, players[i].name, players[i].score);
    }
}
