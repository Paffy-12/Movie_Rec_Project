#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

vector<vector<int>> loadRatingsMatrix(const string& filePath) {
    vector<vector<int>> ratingsMatrix;
    ifstream file(filePath);
    string line;

    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        string value;

        while (getline(ss, value, ',')) {
            row.push_back(stoi(value));
        }

        ratingsMatrix.push_back(row);
    }

    return ratingsMatrix;
}

double calculateSimilarity(const vector<int>& user1, const vector<int>& user2) {
    double dotProduct = 0, norm1 = 0, norm2 = 0;

    for (size_t i = 0; i < user1.size(); ++i) {
        dotProduct += user1[i] * user2[i];
        norm1 += user1[i] * user1[i];
        norm2 += user2[i] * user2[i];
    }

    return dotProduct / (sqrt(norm1) * sqrt(norm2) + 1e-9);
}


double predictRating(const vector<vector<int>>& ratingsMatrix, int targetUser, int movie, const vector<double>& similarities) {
    double weightedSum = 0, similaritySum = 0;

    for (size_t otherUser = 0; otherUser < ratingsMatrix.size(); ++otherUser) {
        if (otherUser == targetUser || ratingsMatrix[otherUser][movie] == 0)
            continue;

        weightedSum += similarities[otherUser] * ratingsMatrix[otherUser][movie];
        similaritySum += fabs(similarities[otherUser]);
    }

    return similaritySum == 0 ? 0 : weightedSum / similaritySum;
}


vector<pair<int, double>> recommendMovies(const vector<vector<int>>& ratingsMatrix, int targetUser, int topN) {
    vector<double> similarities(ratingsMatrix.size());
    for (size_t otherUser = 0; otherUser < ratingsMatrix.size(); ++otherUser) {
        similarities[otherUser] = calculateSimilarity(ratingsMatrix[targetUser], ratingsMatrix[otherUser]);
    }

    vector<pair<int, double>> recommendations;

    for (size_t movie = 0; movie < ratingsMatrix[targetUser].size(); ++movie) {
        if (ratingsMatrix[targetUser][movie] == 0) {
            double predictedRating = predictRating(ratingsMatrix, targetUser, movie, similarities);
            recommendations.emplace_back(movie, predictedRating);
        }
    }

    sort(recommendations.begin(), recommendations.end(), [](auto& a, auto& b) {
        return b.second < a.second;
    });

    if (recommendations.size() > static_cast<size_t>(topN)) {
        recommendations.resize(topN);
    }

    return recommendations;
}

int main() {
    
    string filePath = "ratings.csv";
    vector<vector<int>> ratingsMatrix = loadRatingsMatrix(filePath);

    int targetUser = 0;
    int topN = 3;

    vector<pair<int, double>> recommendations = recommendMovies(ratingsMatrix, targetUser, topN);

    cout << "Top " << topN << " recommendations for User " << targetUser + 1 << ":\n";
    for (const auto& rec : recommendations) {
        cout << "Movie " << rec.first + 1 << " with predicted rating " << rec.second << endl;
    }

    return 0;
}
