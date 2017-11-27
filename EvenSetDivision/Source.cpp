#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>

using namespace std;

/************************************************************************/
/* This class divides set of numbers into two batches, trying to make   */
/* sum of numbers in result batches as equal as possible.               */
/************************************************************************/
class EvenSetDivisionHelper
{
    typedef enum { FIRST, SECOND } BatchType;
    typedef vector <vector <unsigned>> IntMatrix;

public:
    EvenSetDivisionHelper(vector<unsigned> items) : items(items), calculatedBatchesMap(0) {};
    EvenSetDivisionHelper(unsigned items[], size_t size)
        : EvenSetDivisionHelper(vector<unsigned>(items, items + size)) {};

public:
    void calc();
    vector<size_t> getFirstCalculatedBatch() const;
    vector<size_t> getSecondCalculatedBatch() const;

protected:
    vector<size_t> getCalculatedBatch(BatchType batchType) const;

private:
    unsigned getTotalItemsSum();
    vector<BatchType> buildBatchesMap(unsigned targetSum);
    IntMatrix buildKnapsackMatrix(unsigned targetSum);

private:
    vector<unsigned> items;
    vector<BatchType>calculatedBatchesMap;
};

void EvenSetDivisionHelper::calc()
{
    // Calculating total sum of all items
    unsigned totalSum = getTotalItemsSum();

    // Target is to get two batches close to half of total sum.
    this->calculatedBatchesMap = this->buildBatchesMap(totalSum >> 1);
}

unsigned EvenSetDivisionHelper::getTotalItemsSum()
{
    unsigned totalSum = 0;
    for (unsigned &item : items) {
        totalSum += item;
    }

    return totalSum;
}

/**
* Builds map of batches in a form of vector with two possible values: FIRST
* and SECOND.
* Index of every item in the result map corresponds to the index in the
* source EvenSetDivisionHelper::items vector.
*/
vector<EvenSetDivisionHelper::BatchType> EvenSetDivisionHelper::buildBatchesMap(unsigned targetSum)
{
    // Building 0-1 knapsack problem matrix
    IntMatrix matrix = this->buildKnapsackMatrix(targetSum);

    // Initializing result batch matrix with all values of SECOND, which
    // corresponds to "Not selected" in the matrix. So later we'll pick those
    // items that were "selected" and will put them into another (FIRST) batch
    vector<BatchType> resultBatchesMap(items.size(), SECOND);

    // Starting from the bottom-right corner of the matrix
    size_t sumIndex = targetSum;

    // For every item from the last down to the first
    for (size_t itemIndex = items.size(); itemIndex > 0; itemIndex--) {
        // If item with current sum does not equal to the one, right above it
        // then this item was selected
        if (matrix[itemIndex][sumIndex] != matrix[itemIndex - 1][sumIndex]) {
            // Marking current item as FIRST and moving sumIndex (column index)
            // back by the value of current item
            resultBatchesMap[itemIndex - 1] = FIRST;
            sumIndex -= items[itemIndex - 1];
        }

        // We're done if we reached zero value in the matrix
        if ((sumIndex < 1) || (matrix[itemIndex - 1][sumIndex] == 0)) {
            break;
        }
    }

    return resultBatchesMap;
}

/**
 * Calculates matrix like in 0-1 knapsack problem solution, with the   
 * only difference that here weight of an item equals to its value
 *
 * @see https://en.wikipedia.org/wiki/Knapsack_problem for 0-1 knapsack 
 * problem                                                              
 */
EvenSetDivisionHelper::IntMatrix EvenSetDivisionHelper::buildKnapsackMatrix(unsigned targetSum)
{
    IntMatrix matrix(items.size() + 1, IntMatrix::value_type(targetSum + 1));

    for (size_t itemIndex = 0; itemIndex <= items.size(); itemIndex++) {
        for (unsigned sum = 0; sum <= targetSum; sum++) {
            if (itemIndex == 0 || sum == 0) {
                matrix[itemIndex][sum] = 0;
            }
            else if (items[itemIndex - 1] <= sum) {
                matrix[itemIndex][sum] = max(
                    items[itemIndex - 1] + matrix[itemIndex - 1][sum - items[itemIndex - 1]],
                    matrix[itemIndex - 1][sum]
                );
            }
            else {
                matrix[itemIndex][sum] = matrix[itemIndex - 1][sum];
            }
        }
    }

    return matrix;
}

vector<size_t> EvenSetDivisionHelper::getFirstCalculatedBatch() const
{
    return this->getCalculatedBatch(FIRST);
}

vector<size_t> EvenSetDivisionHelper::getSecondCalculatedBatch() const
{
    return this->getCalculatedBatch(SECOND);
}

vector<size_t> EvenSetDivisionHelper::getCalculatedBatch(BatchType batchType) const
{
    // Initializing result batch vector to the maximum possible size
    vector<size_t> batch(this->calculatedBatchesMap.size(), INT_MAX);

    size_t batchSize = 0;
    for (size_t index = 0; index < this->calculatedBatchesMap.size(); index++) {
        if (this->calculatedBatchesMap[index] == batchType) {
            batch[batchSize++] = index;
        }
    }

    // Correcting batch size to the real number of values in the batch
    batch.resize(batchSize);

    return batch;
}

int main()
{
    srand((unsigned)time(nullptr));

    const size_t size = 10;
    for (int run = 0; run < 100; run++) {
        vector<unsigned> input(size, 0);

        for (size_t ind = 0; ind < size; ind++) {
            input[ind] = (rand() % 200) + 100;
        }

         EvenSetDivisionHelper helper(input);
         helper.calc();

        vector<size_t> batch1 = helper.getFirstCalculatedBatch();
        vector<size_t> batch2 = helper.getSecondCalculatedBatch();

        auto printBatch = [&input](vector<size_t> batch, const char* label) -> unsigned {
            cout << label;

            unsigned sum = 0;
            bool firstItem = true;
            for (auto &itemIndex : batch) {
                sum += input[itemIndex];

                if (!firstItem) {
                    cout << " +";
                }
                else {
                    firstItem = false;
                }

                cout << ' ' << input[itemIndex];
            }

            cout << " = " << sum << endl;

            return sum;
        };

        unsigned sum1 = printBatch(batch1, "Batch #1 :");
        unsigned sum2 = printBatch(batch2, "Batch #2 :");

        cout << "Diff: " << abs((long)sum1 - (long)sum2) << "\n\n";
    }

    return 0;
}