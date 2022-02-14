#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
using namespace std;

//Stores how many yahtzees a player has gotten
vector<int> playersNumsOfYahtzees;

//Stores each player's upper score
vector<int> playersUpperScore;

//Defines how fast the AI types
double SPEEDOFAI = 0;

// Current dice position
int curDicePosition[5];

//List of current points of the players
vector<int> curPoints;

//List that describes whether each player can still play
vector<bool> isFinished;

//List that indicates if that player is a human or AI
vector<bool> isPlayer;

//List of possible point positions
// ["1", "2", "3", "4", "5", "6", "Three of a Kind", "Four of a Kind", "Full House", "Small Straight", "Large Straight", "Yahtzee", "Chance"] 
bool possiblePointCombinations[13];

//List of point positions
string pointCombinations[13] = {"1", "2", "3", "4", "5", "6", "Three of a Kind", "Four of a Kind", "Full House", "Small Straight", "Large Straight", "Chance", "Yahtzee"};

//String to index map
map<string, int> indexOfPointPosition;


//List of possible point positions for every player
vector<array<bool, 13>> possiblePointCombinationsForEachPlayer;

//List of possible values such that if the user selects them, it will go to the upperScore
string upperNums[6] = {"1", "2", "3", "4", "5", "6"};

int numOfPlayers = 999;
int numOfAI = 99999999;

void print(map<int, int>& positionsDict) {
    for (int i=0;i<13;i++) {
        if (positionsDict[i]) {
            cout << pointCombinations[i] << ": " << positionsDict[i] << " ";
        }
    }
}

void printWhichToRemove(array<bool, 13> possiblePointCombinations) {
    for (int i=0;i<12;i++) {
        if (possiblePointCombinations[i]) {
            cout << pointCombinations[i] << "   ";
        }
    }
}

void reRollDice(vector<int> selectedDiceToReroll) {
    for (int i : selectedDiceToReroll) {
        curDicePosition[i-1] = rand()%6 + 1;
    }
    // cout << "After rerolling: ";
    // for (int i=0;i<5;i++) {
    //     cout << curDicePosition[i] << " ";
    // }
    // cout << '\n';
}

map<int, int> pointsAvailableFromCurrentDicePosition(int curRoll[5], int playerID) {
    int curRollCopy[5];
    copy(curRoll, curRoll+5, curRollCopy);
    int positions[13];
    // Numbers 1-6
    for (int i=1;i<=6;i++) {
        positions[i-1] = count(curRoll, curRoll+5, i)*i;
    }
    // Three of a Kind
    // Four of a Kind
    bool hasThreeOfAKind = false, hasFourOfAKind = false;
    for (int i=1;i<=6;i++) {
        if (count(curRoll, curRoll+5, i) >= 3) {
            positions[6] = i*3;
            hasThreeOfAKind = true;
        }
        if (count(curRoll, curRoll+5, i) >= 4) {
            positions[7] = i*4;
            hasFourOfAKind = true;
            break;
        }
    }
    if (!hasThreeOfAKind) {
      positions[6] = 0;
    }
    if (!hasFourOfAKind) {
      positions[7] = 0;
    }
    // Full House
    bool hasFullHouse = false;
    for (int i=1;i<=6;i++) {
        for (int j=1;j<=6;j++) {
            if (count(curRoll, curRoll+5, i) == 2 && count(curRoll, curRoll+5, j) == 3) {
                positions[8] = 25;
                hasFullHouse = true;
                break;
            }
        }
    }
    if (!hasFullHouse) {
      positions[8] = 0;
    }

    //Small Straight
    sort(curRoll, curRoll+5);
    for (int i=0;i<=1;i++) {
        bool isStraight = true;
        for (int j=i;j<i+3;j++) {
            if (curRoll[j]+1 != curRoll[j+1]) {
                isStraight = false;
                break;
            }
        }
        if (isStraight) {
            positions[9] = 30;
            break;
        } else {
          positions[9] = 0;
        }
    }

    // Large Straight
    bool isStriaght = true;
    for (int i=0;i<4;i++) {
        if (curRoll[i]+1 != curRoll[i+1]) {
            isStriaght = false;
            break;
        }
    }
    if (isStriaght) {
        positions[10] = 40;
    } else {
      positions[10] = 0;
    }

    // Yahztee
    bool isYahztee = true;
    for (int i=0;i<4;i++) {
        if (curRoll[i] != curRoll[i+1]) {
            isYahztee = false;
            break;
        }
    }
    if (isYahztee) {
        if (playersNumsOfYahtzees[playerID]) {
            positions[12] = 100;
        } else {
            positions[12] = 50;
        }
    } else {
      positions[12] = 0;
    }

    //Chance
    int sum = 0;
    for (int i=0;i<5;i++) {
        sum += curRoll[i];
    }
    positions[11] = sum;

    //Converting to map
    map<int, int> positionsDict;
    for (int i=0;i<13;i++) {
        if (positions[i] && possiblePointCombinationsForEachPlayer[playerID][i]) {
            positionsDict[i] = positions[i];
        }
    }
    copy(curRollCopy, curRollCopy+5, curRoll);
// cout << "Size: " << positionsDict.size() << endl;
    return positionsDict;
}

map<int, int> rollDice(int playerID) {
    // cout << "You rolled: ";
    for (int i=0;i<5;i++) {
        curDicePosition[i] = rand()%6 + 1;
        // cout << curDicePosition[i] << " ";
    }
    // cout << '\n';
    return pointsAvailableFromCurrentDicePosition(curDicePosition, playerID);
}

/*

def printPossiblePointConditionsAndScoreOfEachPlayer():
  for i in range(len(isPlayer)):
    print()
    print("#", i+1)
    print("Score:", curPoints[i])
    print("Still has to get:", possiblePointCombinationsForEachPlayer[i])
    print() 
  return

*/

void printPossiblePointConditionsAndScoreOfEachPlayer() {
  for (int i=0;i<isFinished.size();i++) {
    cout << '\n';
    cout << "#" << i+1 << '\n';
    cout << "Score: " << curPoints[i] << '\n';
    cout << "Still has to get: ";
    for (int j=0;j<possiblePointCombinationsForEachPlayer[i].size();j++) {
        if (possiblePointCombinationsForEachPlayer[i][j]) {
          cout << pointCombinations[j] << "  ";
        }
    }
    cout << "\n\n";
  }
}

void humanRoll(int playerID) {
    string userInput = "";
    // cout << "Enter d to see leaderboard, r to roll, c to clear screen: ";
    // cin >> userInput;
    while (userInput!="d" && userInput!="r" && userInput!="c") {
        cout << "\nEnter d to see leaderboard, r to roll, c to clear screen: ";
        getline(cin, userInput);
    }
    if (userInput == "d") {
        printPossiblePointConditionsAndScoreOfEachPlayer();
        humanRoll(playerID);
    } else if (userInput == "c") {
        cout << "\033c";
        humanRoll(playerID);
    } else if (userInput == "r") {
        map<int, int> positionsDict = rollDice(playerID);
        print(positionsDict);
        cout << '\n';
        string wouldLikeToReroll = "", userSelected="";
        // cout << "Would you like to reroll some dice? (y/n): "; 
        // cin >> wouldLikeToReroll;
        while (wouldLikeToReroll != "y" && wouldLikeToReroll != "n") {
            cout << "Would you like to reroll some dice? (y/n): "; getline(cin, wouldLikeToReroll);
        }
        if (wouldLikeToReroll == "n") {
          cout << "Size: " << positionsDict.size() << endl;
          int amountOfNonZeroElements = 0;
          for (auto& pr : positionsDict) {
            if (pr.second != 0) amountOfNonZeroElements++;
          }
          if (amountOfNonZeroElements == 0) {
              int countOfAvailableSpots = 0;
              for (int i=0;i<13;i++) {
                  if (possiblePointCombinationsForEachPlayer[playerID][i]) {
                    countOfAvailableSpots++;
                  }
              }
              if (countOfAvailableSpots == 1) {
                  isFinished[playerID] = true;
                  for (int i=0;i<50;i++) {
                      cout << '\n';
                  }
                  return;
              } else {
                  printWhichToRemove(possiblePointCombinationsForEachPlayer[playerID]);
                  string userSelectedToSetToZero = "";
                  while (indexOfPointPosition[userSelectedToSetToZero] == 0) {
                      cout << "\nWhich would you like to remove/set to zero: "; getline(cin, userSelectedToSetToZero);
                  }
                  possiblePointCombinationsForEachPlayer[playerID][indexOfPointPosition[userSelectedToSetToZero]-1] = false;
                  return;
              }
          }
            // cout << "Which would you like to fill in: "; getline(cin, userSelected);
            // // userSelected.pop_back();
            // while (indexOfPointPosition[userSelected]==0) {
            //   cout << "Which would you like to fill in: ";  getline(cin, userSelected);
            //   cout << "Input: " << userSelected << endl;
            // }
        } else if (wouldLikeToReroll == "y") {
          bool isDiceValid = false;
          vector<string> diceToBeRerolledStringForm;
          vector<int> diceToBeRerolled;
          while (!isDiceValid) {
            diceToBeRerolled.clear();
            isDiceValid = true;
            string input; cout << "What dice would you like to reroll: "; 
            // cin.ignore();
            getline(cin, input);
            boost::split(diceToBeRerolledStringForm, input, boost::is_any_of(" ")); // Taken from https://www.geeksforgeeks.org/boostsplit-c-library/
            for (string dice : diceToBeRerolledStringForm) {
              if (dice.size() > 1) {
                isDiceValid = false;
                break;
              }
              if (dice[0] - '0' > 0 && dice[0] - '0' < 6 && count(diceToBeRerolledStringForm.begin(), diceToBeRerolledStringForm.end(), dice) == 1) {
                diceToBeRerolled.push_back(dice[0] - '0');
                continue;
              } else {
                isDiceValid = false;
                break;
              }
            }  
          }
          reRollDice(diceToBeRerolled);
          positionsDict = pointsAvailableFromCurrentDicePosition(curDicePosition, playerID);
          cout << '\n';
          print(positionsDict);
          cout << '\n';


          // // Ask again if they want to reroll
          string wouldLikeToReroll = "", userSelected="";
          // cout << "Would you like to reroll some dice? (y/n): "; 
          // cin >> wouldLikeToReroll;
          while (wouldLikeToReroll != "y" && wouldLikeToReroll != "n") {
              cout << "Would you like to reroll some dice? (y/n): "; getline(cin, wouldLikeToReroll);
          }
          if (wouldLikeToReroll == "n") {
            cout << "Size: " << positionsDict.size() << endl;
            int amountOfNonZeroElements = 0;
            for (auto& pr : positionsDict) {
              if (pr.second != 0) amountOfNonZeroElements++;
            }
            if (amountOfNonZeroElements == 0) {
                int countOfAvailableSpots = 0;
                for (int i=0;i<13;i++) {
                    if (possiblePointCombinationsForEachPlayer[playerID][i]) {
                      countOfAvailableSpots++;
                    }
                }
                if (countOfAvailableSpots == 1) {
                    isFinished[playerID] = true;
                    for (int i=0;i<50;i++) {
                        cout << '\n';
                    }
                    return;
                } else {
                    printWhichToRemove(possiblePointCombinationsForEachPlayer[playerID]);
                    string userSelectedToSetToZero = "";
                    while (indexOfPointPosition[userSelectedToSetToZero] == 0) {
                        cout << "\nWhich would you like to remove/set to zero: "; getline(cin, userSelectedToSetToZero);
                    }
                    possiblePointCombinationsForEachPlayer[playerID][indexOfPointPosition[userSelectedToSetToZero]-1] = false;
                    return;
                }
            }
              // cout << "Which would you like to fill in: "; getline(cin, userSelected);
              // // userSelected.pop_back();
              // while (indexOfPointPosition[userSelected]==0) {
              //   cout << "Which would you like to fill in: ";  getline(cin, userSelected);
              //   cout << "Input: " << userSelected << endl;
              // }
          } else if (wouldLikeToReroll == "y") {
            bool isDiceValid = false;
            vector<string> diceToBeRerolledStringForm;
            vector<int> diceToBeRerolled;
            while (!isDiceValid) {
              diceToBeRerolled.clear();
              isDiceValid = true;
              string input; cout << "What dice would you like to reroll: "; 
              // cin.ignore();
              getline(cin, input);
              boost::split(diceToBeRerolledStringForm, input, boost::is_any_of(" ")); // Taken from https://www.geeksforgeeks.org/boostsplit-c-library/
              for (string dice : diceToBeRerolledStringForm) {
                if (dice.size() > 1) {
                  isDiceValid = false;
                  break;
                }
                if (dice[0] - '0' > 0 && dice[0] - '0' < 6 && count(diceToBeRerolledStringForm.begin(), diceToBeRerolledStringForm.end(), dice) == 1) {
                  diceToBeRerolled.push_back(dice[0] - '0');
                  continue;
                } else {
                  isDiceValid = false;
                  break;
                }
              }  
            }
            reRollDice(diceToBeRerolled);
            positionsDict = pointsAvailableFromCurrentDicePosition(curDicePosition, playerID);
            cout << '\n';
            print(positionsDict);
            cout << '\n';
            
            int amountOfNonZeroElements = 0;
            for (auto& pr : positionsDict) {
              if (pr.second != 0) amountOfNonZeroElements++;
            }
            if (amountOfNonZeroElements == 0) {
                int countOfAvailableSpots = 0;
                for (int i=0;i<13;i++) {
                    if (possiblePointCombinationsForEachPlayer[playerID][i]) {
                      countOfAvailableSpots++;
                    }
                }
                if (countOfAvailableSpots == 1) {
                    isFinished[playerID] = true;
                    for (int i=0;i<50;i++) {
                        cout << '\n';
                    }
                    return;
                } else {
                    printWhichToRemove(possiblePointCombinationsForEachPlayer[playerID]);
                    string userSelectedToSetToZero = "";
                    while (indexOfPointPosition[userSelectedToSetToZero] == 0) {
                        cout << "\nWhich would you like to remove/set to zero: "; getline(cin, userSelectedToSetToZero);
                    }
                    possiblePointCombinationsForEachPlayer[playerID][indexOfPointPosition[userSelectedToSetToZero]-1] = false;
                    return;
                }
            }
            amountOfNonZeroElements = 0;
            for (auto& pr : positionsDict) {
              if (pr.second != 0) amountOfNonZeroElements++;
            }
            if (amountOfNonZeroElements == 0) {
                int countOfAvailableSpots = 0;
                for (int i=0;i<13;i++) {
                    if (possiblePointCombinationsForEachPlayer[playerID][i]) {
                      countOfAvailableSpots++;
                    }
                }
                if (countOfAvailableSpots == 1) {
                    isFinished[playerID] = true;
                    for (int i=0;i<50;i++) {
                        cout << '\n';
                    }
                    return;
                } else {
                    printWhichToRemove(possiblePointCombinationsForEachPlayer[playerID]);
                    string userSelectedToSetToZero = "";
                    while (indexOfPointPosition[userSelectedToSetToZero] == 0) {
                        cout << "\nWhich would you like to remove/set to zero: "; getline(cin, userSelectedToSetToZero);
                    }
                    possiblePointCombinationsForEachPlayer[playerID][indexOfPointPosition[userSelectedToSetToZero]-1] = false;
                    return;
                }
            }
          }
        }

      // Have the user select which one they want
      userSelected = "";
      while (indexOfPointPosition[userSelected]==0 || positionsDict[indexOfPointPosition[userSelected]-1] == 0) {
        cout << "Which would you like to fill in: "; getline(cin, userSelected);
      }

      // Process userSelection
      if (find(upperNums, upperNums+6, userSelected) != upperNums+6) {
        playersUpperScore[playerID] += positionsDict[indexOfPointPosition[userSelected]-1];
      }
      curPoints[playerID] += positionsDict[indexOfPointPosition[userSelected]-1];
      possiblePointCombinationsForEachPlayer[playerID][indexOfPointPosition[userSelected]-1] = false;
      cout << "Points: " << curPoints[playerID] << endl;
    }
}

int mode(int data[5]) {
  map<int, int> mp;
  for (int i=0;i<5;i++) {
    mp[data[i]]++;
  }
  int indexOf = 0;
  int value = -1;
  for (auto& pr : mp) {
    if (pr.second > value) {
      value = pr.second;
      indexOf = pr.first;
    }
  }
  return indexOf;
}

int inARow(int curDice[5]) {
  set<int> st;
  vector<int> uniqueDice;
  for (int i=0;i<5;i++) {
    st.insert(curDice[i]);
  }
  for (int uniqueDie : st) {
    uniqueDice.push_back(uniqueDie);
  }

  int length = 1, maybeLength=1;
  for (int i=0;i<uniqueDice.size()-1;i++) {
    if (uniqueDice[i]+1 == uniqueDice[i+1]) {
      maybeLength++;
    } else {
      if (maybeLength > length) length = maybeLength;
      maybeLength=1;
    }
  }
  if (maybeLength > length) length = maybeLength;
  return length;
}

bool iter5OfAI(array<bool, 13> positionsPosi, map<int, int>& positionsDict, int indexOfMaxValue, int turn=2) {
  if (turn == 1) {
    if (positionsDict[indexOfMaxValue] == 9 && positionsPosi[10]) return false;
    else if (positionsDict[indexOfMaxValue] == 7) return false;
    else if (positionsDict[indexOfMaxValue] == 6 && positionsPosi[7]) return false;
    else if (positionsDict[indexOfMaxValue] < 6) return false;
  } 
  if (positionsDict[indexOfMaxValue] >= 20 && indexOfMaxValue != 11) return true;
  return false;
}

vector<int> iter5OfAIChoosingDiceToRoll(array<bool, 13> positionsPosi, int curDice[5]) {
  vector<int> dieToRandomize;
  vector<int> curDiceVector;
  for (int i=0;i<5;i++) {
    curDiceVector.push_back(curDice[i]);
  }
  int mostCommon = mode(curDice);
  set<int> diffDice; vector<int> sortedList;
  for (int die : curDiceVector) diffDice.insert(die);
  for (int die : diffDice) sortedList.push_back(die);

  if (inARow(curDice)<3 || ((count(curDiceVector.begin(), curDiceVector.end(), mostCommon) >= 2 && diffDice.size() <= 3) || !(positionsPosi[9]||positionsPosi[10])) ) {
    if (count(curDiceVector.begin(), curDiceVector.end(), mostCommon) == 2 && positionsPosi[8] && diffDice.size() < 3) {
      for (int die : diffDice) {
        if (count(curDiceVector.begin(), curDiceVector.end(), die) == 1) {
          dieToRandomize.push_back(die);
          return dieToRandomize;
        }
      }
    }
    for (int i=0;i<5;i++) {
      if (curDice[i] != mostCommon) {
        dieToRandomize.push_back(i+1);
      }
    }
  } else {
    int startIndex = 0, maybeStartIndex = 0, maybeEndIndex = 0, endIndex = 0, maybeLength = 1, length = 1;
    for (int i=0;i<sortedList.size()-1;i++) {
      if (sortedList[i]+1 == sortedList[i+1]) {
        maybeLength+=1;
        maybeEndIndex=i+1;
      } else {
        if (maybeLength > length) {
          length = maybeLength;
          startIndex = maybeStartIndex;
          endIndex = maybeEndIndex;
        }
        maybeStartIndex = i+1;
        maybeEndIndex = i+1;
        maybeLength = 1;
      }
    }
    if (maybeLength > length) {
      length = maybeLength;
      startIndex = maybeStartIndex;
      endIndex = maybeEndIndex;
    }
    vector<int> copyList, copyList2;
    for (int i=0;i<5;i++) {
      copyList.push_back(curDice[i]);
      copyList2.push_back(curDice[i]);
    }
    for (int i=startIndex;i<endIndex+1;i++) {
      for (int j=0;j<copyList.size();j++) {
        if (copyList[j]==sortedList[i]) {
          copyList.erase(copyList.begin()+j);
          break;
        }
      }
    }
    for (int die : copyList) {
      int position = -1;
      for (int i=0;i<copyList2.size();i++) {
        if (copyList2[i] == die) {
          position = i;
          break;
        }
      }
      dieToRandomize.push_back(position+1);
      copyList2[position] = -1;
    }
  }
  return dieToRandomize;
}

void AIRoll(int playerID) {
  auto positionsDict = rollDice(playerID);
  int maxValue = 0, indexOfMaxValue = 0;
  for (auto& pr : positionsDict) {
    if (pr.second > maxValue) {
      maxValue = pr.second;
      indexOfMaxValue = pr.first;
    }
  }
  if (maxValue && iter5OfAI(possiblePointCombinationsForEachPlayer[playerID], positionsDict, indexOfMaxValue, 1)) {
    ;
  } else {
    maxValue = 0;
    vector<int> dieToRandomize;
    dieToRandomize = iter5OfAIChoosingDiceToRoll(possiblePointCombinationsForEachPlayer[playerID], curDicePosition);
    reRollDice(dieToRandomize);
    positionsDict = pointsAvailableFromCurrentDicePosition(curDicePosition, playerID);

    // If AI wants to roll again (this is the last turn)
    for (auto& pr : positionsDict) {
      if (pr.second > maxValue) {
        maxValue = pr.second;
        indexOfMaxValue = pr.first;
      }
    }
    if (maxValue && iter5OfAI(possiblePointCombinationsForEachPlayer[playerID], positionsDict, maxValue, 1)) {
      ;
    } else {
      maxValue = 0;
      vector<int> dieToRandomize;
      dieToRandomize = iter5OfAIChoosingDiceToRoll(possiblePointCombinationsForEachPlayer[playerID], curDicePosition);
      reRollDice(dieToRandomize);
      positionsDict = pointsAvailableFromCurrentDicePosition(curDicePosition, playerID);
      int count = 0;

      for (auto& pr : positionsDict) {
        if (pr.second) count++;
        if (pr.second > maxValue) {
          maxValue = pr.second;
          indexOfMaxValue = pr.first;
        }
      }   

      if (maxValue) {
        if (indexOfMaxValue == 11) {
          if (count == 1) {
            ;
          } else {
            positionsDict[indexOfMaxValue] = 0;
            maxValue = 0;
            for (auto& pr : positionsDict) {
              if (pr.second > maxValue) {
                maxValue = pr.second;
                indexOfMaxValue = pr.first;
              }
            }  
          }
        }
      }

      if (!maxValue) {
        int count = 0;
        int lowest = -1;
        for (int i=0;i<13;i++) {
          if (possiblePointCombinationsForEachPlayer[playerID][i]) {
            count++;
            if (lowest == -1) {lowest=i;}
          }
        }
        if (count <= 1) {
          isFinished[playerID] = true;
          return;
        } else {
          possiblePointCombinationsForEachPlayer[playerID][lowest] = false;
          return;
        }
      }
    }
  }
  if (find(upperNums, upperNums+6, pointCombinations[indexOfMaxValue]) != upperNums+6) {
    playersUpperScore[playerID] += maxValue;
  }

  // cout << "ADDING: " << maxValue << endl;
  // cout << "WHAT IT IS: " << pointCombinations[indexOfMaxValue] << endl;

  curPoints[playerID] += maxValue;
  possiblePointCombinationsForEachPlayer[playerID][indexOfMaxValue]=false;
}

void endScreen() {
  for (int i=0;i<isPlayer.size();i++) {
    cout << '\n';
    cout << "#" << i+1;
    if (isPlayer[i]) {
      cout << " - Human\n";
    } else {
      cout << " - AI\n";
    }
    cout << "Score: " << curPoints[i] << '\n';
  }
  cout << '\n';
}

int main() {
  srand(time(NULL));
  for (int i=0;i<13;i++) {
      indexOfPointPosition[pointCombinations[i]] = i+1;
  }
  cout << "How many human players: ";
  int numOfPlayers; cin >> numOfPlayers; cin.clear(); cin.ignore();
  cout << "How many AI players: ";
  int numOfAI; cin >> numOfAI; cin.clear(); cin.ignore();

  for (int i=0;i<numOfPlayers;i++) {
    isPlayer.push_back(true);
  }

  for (int i=0;i<numOfAI;i++) {
    isPlayer.push_back(false);
  }

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  shuffle(isPlayer.begin(), isPlayer.end(), default_random_engine(seed));


  for (int i=0;i<isPlayer.size();i++) {
    possiblePointCombinationsForEachPlayer.push_back({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
    playersUpperScore.push_back(0);
    curPoints.push_back(0);
    isFinished.push_back(false);
    playersNumsOfYahtzees.push_back(0);
  }
  bool isGameRunning = true;
  int howManyStillPlaying = 0;
  while (isGameRunning) {
    // cout << "STILL PLAYING\n";
    howManyStillPlaying = 0;
    for (int i=0;i<isPlayer.size();i++) {
      if (!isFinished[i]) {
        if (!possiblePointCombinationsForEachPlayer[i][12]) {
          possiblePointCombinationsForEachPlayer[i][12] = true;
          playersNumsOfYahtzees[i]++;
        }
        howManyStillPlaying++;
        if (isPlayer[i]) {
          humanRoll(i);
        } else {
          AIRoll(i);
        }
      }
    }
    if (!howManyStillPlaying) {
      isGameRunning = false;
    }
  }

  for (int i=0;i<isPlayer.size();i++) {
    if (playersUpperScore[i]>=63) {
      curPoints[i]+=35;
    }
  }

  ofstream outputFile;
  outputFile.open("output.txt");

  sort(curPoints.begin(), curPoints.end(), greater<int>());

  for (int i=0;i<isPlayer.size();i++) {
    outputFile << curPoints[i] << ", ";
  }
  outputFile.close();

  // endScreen();

  return 0;
}



/*
#Helper functions 
from AI import iter5OfAI, iter5OfAIChoosingDiceToRoll

def printPossiblePointConditionsAndScoreOfEachPlayer():
  for i in range(len(isPlayer)):
    print()
    print("#", i+1)
    print("Score:", curPoints[i])
    print("Still has to get:", possiblePointCombinationsForEachPlayer[i])
    print() 
  return


def endScreen():
  for i in range(len(isPlayer)):
    print()
    print("#", i+1, sep="", end="")  
    if isPlayer[i]:
      print(" - Human") 
    else:
      print(" - AI")
    print("Score:", curPoints[i])
    print() 
  return


#Driver Code 
while not numOfPlayers.isnumeric() or int(numOfPlayers) >= 10:
  numOfPlayers = input("How many human players (1-9): ")

while not numOfAI.isnumeric() or int(numOfAI) >= 1000000 or int(numOfAI) + int(numOfPlayers) <= 1:
  numOfAI = input("How many AI players (1-9): ")

for _ in range(int(numOfPlayers)):
  isPlayer.append(True) 

for _ in range(int(numOfAI)):
  isPlayer.append(False)

# rn.shuffle(isPlayer)
# print("Order of players:")
# for item in isPlayer:
#   if item:
#     print("Human", end=" ")
#   else:
#     print("AI", end=" ")
# print() 


for _ in range(len(isPlayer)):
  curPoints.append(0) 
  isFinished.append(False) 
  playersNumsOfYahtzees.append(0)
  playersUpperScore.append(0) 
  possiblePointCombinationsForEachPlayer.append(possiblePointCombinations.copy()) 


while False in isFinished:
  for i in range(len(isFinished)):
    if isFinished[i]:
      continue

    if "Yahtzee" not in possiblePointCombinationsForEachPlayer[i]:
      playersNumsOfYahtzees[i]+=1
      possiblePointCombinationsForEachPlayer[i].append("Yahtzee") 

    if isPlayer[i]:
    #   print("\n\n-------------------------Human #", i+1, "-------------------------", sep="")
      humanRoll(i) 
    else:
    #   print("\n\n-------------------------AI #", i+1, "-------------------------", sep="")
      AIRoll(i) 

for i in range(len(playersUpperScore)):
  if playersUpperScore[i] >= 63:
    curPoints[i] += 35

# endScreen()
# print(sorted(curPoints)[::-1])
# print()
# print(sorted(playersUpperScore)[::-1])
# print(sorted(playersNumsOfYahtzees)[::-1])
f = open("yahtzeeData.txt", "w")
# f.write(str(sorted(curPoints)[::-1]))
f.write(str(sorted(playersUpperScore)[::-1]))
# f.write(str(sorted(playersNumsOfYahtzees)[::-1]))
# for item in sorted(playersNumsOfYahtzees)[::-1]:
#   f.write(str(item) + ", ")
f.close()
*/