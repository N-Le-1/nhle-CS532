#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
int sumOfDigits(int n);
int UABMaxMinDiff(int arr[], int size);
int* replaceEvenWithZero(int arr[], int size);
bool perfectSquare(int n);
int countVowels(char* s);

/*
main function to test the functions in the assignment
calls each function with various test cases (from the assignment) and prints the results

How to Run:
1. Go to the terminal and navigate to the directory containing this file.
2. Compile the code using the command: gcc -o nhle_HW01 nhle_HW01.c
3. Run the compiled program using the command: ./nhle_HW01
*/
int main() {
    //1. sumOfDigits
    printf("=== Testing sumOfDigits ===\n");
    printf("n=123 -> Expected: 6  | Actual: %d\n", sumOfDigits(123));
    printf("n=405 -> Expected: 9  | Actual: %d\n", sumOfDigits(405));
    printf("n=0   -> Expected: -1 | Actual: %d\n", sumOfDigits(0));
    printf("n=7   -> Expected: 7  | Actual: %d\n", sumOfDigits(7));
    printf("n=-308-> Expected: -1 | Actual: %d\n\n", sumOfDigits(-308));


    //2. UABMaxMinDiff
    printf("=== Testing UABMaxMinDiff ===\n");
    int arr1[] = {3, 7, 2, 9};
    printf("arr = [3, 7, 2, 9] -> Expected: 7 | Actual: %d\n", UABMaxMinDiff(arr1, 4));
    
    int arr2[] = {5, 5, 5, 5, 5, 5};
    printf("arr = [5,5,5,5,5,5] -> Expected: 0 | Actual: %d\n", UABMaxMinDiff(arr2, 6));
    
    int arr3[] = {-2, 4, -1, 6, 5};
    printf("arr = [-2, 4, -1, 6, 5] -> Expected: 8 | Actual: %d\n\n", UABMaxMinDiff(arr3, 5));


    //3. replaceEvenWithZero
    printf("=== Testing replaceEvenWithZero ===\n");
    
    int arr4[] = {1, 2, 3, 4};
    int* res1 = replaceEvenWithZero(arr4, 4);
    printf("arr = [1, 2, 3, 4] -> Expected: [1, 0, 3, 0] | Actual: [");
    for (int i = 0; i < 4; i++) {
        printf("%d", res1[i]);
        if (i < 3) printf(", ");
    }
    printf("]\n");
    free(res1);

    // Test 2
    int arr5[] = {2, 4, 6};
    int* res2 = replaceEvenWithZero(arr5, 3);
    printf("arr = [2, 4, 6]    -> Expected: [0, 0, 0]    | Actual: [");
    for (int i = 0; i < 3; i++) {
        printf("%d", res2[i]);
        if (i < 2) printf(", ");
    }
    printf("]\n");
    free(res2);

    // Test 3
    int arr6[] = {1, 3, 5};
    int* res3 = replaceEvenWithZero(arr6, 3);
    printf("arr = [1, 3, 5]    -> Expected: [1, 3, 5]    | Actual: [");
    for (int i = 0; i < 3; i++) {
        printf("%d", res3[i]);
        if (i < 2) printf(", ");
    }
    printf("]\n\n");
    free(res3);


    //4. perfectSquare
    printf("=== Testing perfectSquare ===\n");
    printf("n = 16 -> Expected: True  | Actual: %s\n", perfectSquare(16) ? "True" : "False");
    printf("n = 15 -> Expected: False | Actual: %s\n", perfectSquare(15) ? "True" : "False");
    printf("n = 25 -> Expected: True  | Actual: %s\n", perfectSquare(25) ? "True" : "False");
    printf("n = 36 -> Expected: True  | Actual: %s\n\n", perfectSquare(36) ? "True" : "False");


    //5. countVowels
    printf("=== Testing countVowels ===\n");
    printf("s = \"Hello World\" -> Expected: 3 | Actual: %d\n", countVowels("Hello World"));
    printf("s = \"UAB CS\"      -> Expected: 2 | Actual: %d\n", countVowels("UAB CS"));
    printf("s = \"Python\"      -> Expected: 1 | Actual: %d\n", countVowels("Python"));
    printf("s = \"aeiou\"       -> Expected: 5 | Actual: %d\n\n", countVowels("aeiou"));

    return 0;
}

/*
1. sumOfDigits 

Description: This function takes a positive integer n and returns the sum of its digits. 
If n is zero or negative, it returns -1.

Input: positive integer n
Output: sum of digits of n, or -1 if n is zero or negative
*/

int sumOfDigits(int n) {
    //base case for zero or negative numbers
    if (n <= 0) {
        return -1;
    }
    // Initialize sum to 0 and iterate through the digits of n
    int sum = 0;
    while (n > 0) {
        // Add the last digit of n to sum and remove the last digit from n
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

/*
2. UABMaxMinDiff
Description: This function takes an array of integers and its size, and returns the difference 
between the maximum and minimum values in the array.

Input: array of integers and its size
Output: difference between the maximum and minimum values in the array
*/
int UABMaxMinDiff(int arr[], int size) {
    //base case
    if (size <= 0) return 0;

    // Initialize max and min to the first element of the array
    int max = arr[0];
    int min = arr[0];

    // Iterate through the array to find the maximum and minimum values
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return max - min;
}

/*
3. replaceEvenWithZero
Description: This function takes an array of integers and its size, and returns a new array where 
all even numbers are replaced with zero.

Input: array of integers and its size
Output: new array with even numbers replaced by zero
*/
int* replaceEvenWithZero(int arr[], int size) {
    //must use malloc to be able to return the new array
    int* newArr = (int*)malloc(size * sizeof(int));
    if (newArr == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Iterate through the original array and replace 
    //even numbers with zero in the new array
    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) {
            newArr[i] = 0;
        } else {
            newArr[i] = arr[i];
        }
    }
    return newArr;
}

/*
4. perfectSquare
Description: This function takes an integer n and returns true if n is a perfect square, and
false otherwise.

Input: integer n
Output: true if n is a perfect square, false otherwise
*/

bool perfectSquare(int n) {
    //base case
    if (n < 0) return false;
    
    // Iterate through integers starting from 0 and check if their squares are equal to n
    for (long long i = 0; i * i <= n; i++) {
        if (i * i == n) {
            return true;
        }
    }
    return false;
}

/*
5. countVowels
Description: This function takes a string and returns the number of vowels in it.

Input: string s
Output: number of vowels in the string
*/
int countVowels(char* s) {
    // Initialize count to 0 and iterate through the characters of the string
    int count = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        char c = tolower(s[i]);  //use tolower to count both uppercase and lowercase vowels
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}