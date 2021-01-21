// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    FBullCowCount Count;
    

    // Make array from .txt file containing all the isograms.
    // Doing this here turns huge compile time into run time
    // as the words are loading in at BeginPlay. 
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("Word_Lists/Isograms_List.txt");
    FFileHelper::LoadFileToStringArray(WordList, *WordListPath);
    
    Isograms = GetValidWords(WordList);    // remove invalid words from list

    SetupGame();  

    // PrintLine(TEXT("The number of possible words is %i"), GetValidWords(WordList).Num());   // DEBUG LINE
}



////////// Wait for player to input ////////////////

void UBullCowCartridge::OnInput(const FString& PlayerInput) // Wait for player input 
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        ProcessGuess(PlayerInput);
    }    
}


/////////// Set up the game ///////////

void UBullCowCartridge::SetupGame()
{
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() -1)]; 
    Lives = HiddenWord.Len();
    bGameOver = false;
    
    // Welcome player to game
    PrintLine(TEXT("Welcome to the Bull-Cow Word Guessing Game")); 
    PrintLine(TEXT("Guess the %i letter word! You have %i lives."), HiddenWord.Len(), Lives);
    PrintLine(TEXT("Type your guess and \npress enter to continue..."));
    // PrintLine(TEXT("The Hidden Word is: %s."), *HiddenWord);    // DEBUG LINE
}

///////// End the game ////////////

void UBullCowCartridge::EndGame()
{
    bGameOver = true; 
    PrintLine(TEXT("Press enter to play again..."));
}



////////// Process the Player Guess ///////////////

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        ClearScreen();
        PrintLine(TEXT("You have won!"));
        EndGame();
        return;
    } 

    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("The word has %i letters. You still have %i lives remaining. \nTry again!"), HiddenWord.Len(), Lives);
        return;
    }

    if (!IsIsogram(Guess))  // check if guess is an isogram
    {
        PrintLine(TEXT("No repeating letters! You still have %i lives remaining. \nGuess again!"), Lives);
        return;
    }
    
    --Lives;    // decriment life for wrong answer

    FBullCowCount Score = GetBullsCows(Guess);
    PrintLine(TEXT("You got %i Bull(s) and %i Cow(s)"), Score.Bulls, Score.Cows);

    if (Lives > 1)
    {
        PrintLine(TEXT("You have %i lives remaining...\nTry again!"), Lives);
        return;
    }

    if (Lives == 1 )
    {
        PrintLine(TEXT("You have %i life remaining...\nUse it wisely!"), Lives);
        return;
    }
    else
    {
        ClearScreen();
        PrintLine(TEXT("Sorry, you've ran out of lives. \nThe word you were looking for was \"%s\"."), *HiddenWord);
        EndGame();
        return;
    } 
}



/////////// Check Word is an isogram ///////////////

bool UBullCowCartridge::IsIsogram(const FString& Word) const  // add const to functions which DO NOT ALTER member variables.This is good practice.
{
    for (int32 i = 0; i < Word.Len() -1; i++)
    {
        for (int32 j = i+1; j < Word.Len(); j++)
        {
            if (Word[i] == Word[j])
            {
                return false;
            }
        }
    }
    return true;
}



//////////// Remove words in list that are too long ///////////

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& List) const
{
    TArray<FString> ValidWords; 

    for (FString Isos : WordList)
    {
        if (Isos.Len() >= 4 && Isos.Len() <= 6)
        {
            ValidWords.Emplace(Isos);
        }
    }
    return ValidWords;
}


//////// Get Bulls and Cows ///////////

FBullCowCount UBullCowCartridge::GetBullsCows(const FString& Guess) const
{
    FBullCowCount Count;
    for (int32 i = 0; i < Guess.Len(); i++)
    {
        if (Guess[i] == HiddenWord[i])
        {
            Count.Bulls++;
            continue;   // continue moves on to the next iteration
        }
        for (int32 j = 0; j < HiddenWord.Len(); j++)
        {
            if (Guess[i] == HiddenWord[j])
            {
                Count.Cows++;
                break;  // break gets out of the for loop
            }
        }
    }
    return Count;
}