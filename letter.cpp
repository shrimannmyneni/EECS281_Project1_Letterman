// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9
    #include <vector>
    #include <string>
    #include <deque>
    #include <sstream>
    #include <functional>
    #include <getopt.h> // parse command line options
    #include <iostream> //cout, cerr
    #include <algorithm>
    
    struct dictWord{
        std::string word;
        bool discovered;
        int prev;

        dictWord(const std::string &WORD):word(WORD){
            discovered = false;
            prev = -1;
        }

    };

    class Letterman {
        private:
        std::string beginWord, endWord;
        std::string outputFormat = "W";

        bool changeMode = false, lengthMode = false, swapMode = false;

        bool queueMode = false, stackMode = false;

        std::vector <dictWord> dict;
        std::deque <int> sc;

        int current;
        char dictType;

        size_t num_words_that_entered_SC = 0;
        
        int numWords;

        public:
        void getOptions (int argc, char** argv){

            unsigned int numSpecsOfQS = 0;

            int option = 0;
            int opt_index = 0;
            opterr = false; // to disable automatic error printing

             struct option longopts[] = {
                {"help", no_argument, nullptr, 'h'},
                {"queue", no_argument, nullptr, 'q'},
                {"stack", no_argument, nullptr, 's'},
                {"begin", required_argument, nullptr, 'b'},
                {"end", required_argument, nullptr, 'e'},
                {"output", optional_argument /*2*/, nullptr, 'o'},
                {"change", no_argument, nullptr, 'c'},
                {"length", no_argument, nullptr, 'l'},
                {"swap", no_argument, nullptr, 'p'},
                { nullptr, no_argument, nullptr, '\0' }
            };

            while ((option = getopt_long(argc, argv, 
            "hqsb:e:o:clp", longopts, &opt_index)) != -1) {
            switch (option) {
            

            case 'h'://help
                std::cerr << "This program is designed to help Letterman find the best path\n";
                exit(0);
                break;
        
            case 'q'://queue
                std::cerr << "queue mode\n";
                queueMode = true;
                numSpecsOfQS++;
                break;
            
            case 's'://stack
                std::cerr << "stack mode\n";
                stackMode = true;
                numSpecsOfQS++;
                break;
            
            case 'b'://begin
                beginWord = optarg;
                std::cerr << "beginWord: " << beginWord << "\n";
                break;
            
            case 'e'://end
                endWord = optarg;
                std::cerr << "endWord: " << endWord << "\n";
                break;
            
            case 'o'://output
                outputFormat = optarg;
                std::cerr << "outputFormat: " << outputFormat << "\n";
                break;
                
            case 'c'://change
                std::cerr << "Change Mode\n";
                changeMode = true;
                break;
            
            case 'l'://length
                std::cerr << "Length mode\n";
                lengthMode = true;
                break;
        
            case 'p'://swap
                std::cerr << "Swap mode\n";
                swapMode = true;
                break;
        
            default: 
                std::cerr << "Unknown or incorrect option\n";
                exit(1);
            
        } // switch
    } // end of while loop 

    //error checking
    if ((stackMode == queueMode)||(numSpecsOfQS != 1)){
            std::cerr << "Either --stack or --queue must be specified exactly once\n";
            exit(1);
        }
        if (!changeMode && !lengthMode && !swapMode){
            std::cerr << "Must specify atleast one of change, length, or swap\n";
            exit(1);
        }
        if (beginWord.empty() || endWord.empty()){
            std::cerr << "Both --begin and --end must be specified\n";
            exit(1);
        }

        if (outputFormat != "W" && outputFormat != "M"){
            std::cerr << "Incorrect Output need to specify\n";
            exit(1);
        }

        if ((changeMode || swapMode) && !lengthMode && (beginWord.size() != endWord.size())){
            std::cerr << "The --change/-c and/or --swap/-p flags are specified, but --length/-l is not, and the begin or/and end words do not match in length (creating an impossible situation).\n";
            exit(1);
        }

    }// end of get options


        void readInput(){  //begin of readInput
            std::cin >> dictType >> numWords;
            dict.reserve(static_cast<size_t>(numWords));

            std::string line;
            dict.push_back(dictWord(beginWord)); // begin word index 0
            dict.push_back(dictWord(endWord)); // end word index 1

            while (getline(std::cin,line)){
                if(!line.empty() && (line[0] != '/') && (line[1] != '/')){
                    //gets rid of trailing & leading whitespace
                    std::istringstream iss;
                    iss.str(line);
                    iss >> line;
                    if (dictType == 'S'){
                        processSimpleWord(line);
                    } else if (dictType == 'C'){
                        processComplexWord(line);
                    } else {
                        std::cerr << "S or C must be specified\n";
                        exit(1);
                    }
                }
            }
            
            //find if both begin and end word in dictionary
            bool begin_word_exists_in_dictionary = false;
            bool end_word_exists_in_dictionary = false;
                if (dict[0].word == beginWord){
                    begin_word_exists_in_dictionary = true;
                }
                if (dict[1].word == endWord){
                    end_word_exists_in_dictionary = true; 
                }
            if (!begin_word_exists_in_dictionary || !end_word_exists_in_dictionary){
                std::cerr << "begin and/or end word does not exist in the dictionary\n";
                exit(1);
            }
        }//end of read input



    void processSimpleWord(std::string& word){
        if ((!lengthMode && (word.size() != endWord.size())) || (word == beginWord) || (word == endWord)){
            return;
        }
        dict.push_back(dictWord(word));
    }

    void processComplexWord (std::string& word){
        //reversal (&)
        if (word.back() == '&'){
            processReversal(word);
        }
        //insert-each ([])
        else if (word.find('[') != std::string::npos) {
            processInsertEach(word);
        }
        //swap (!)
        else if (word.find('!') != std::string::npos){
            processSwap(word);
        }
        //double (?)
        else if (word.find('?') != std::string::npos){
            processDouble(word);
        } else {
            //no special markers
            processSimpleWord(word);
        }
    }

        void processReversal(std::string& word){
            //if theres only an ampersand
            if (word.size() < 2){
                return;
            }
            //if there is only one character to reverse
            if (word.size() == 2){
                dict.push_back(dictWord(word.substr(0,1)));
                return;
            }
            word.pop_back();
            if (word != endWord && word != beginWord){
            dict.push_back(dictWord(word));
            }
            std::reverse(word.begin(), word.end());
            if (word != endWord && word != beginWord){
            dict.push_back(dictWord(word));
            }
        }

        void processInsertEach(const std::string& word){
            //no ending bracket
            size_t pos1 = word.find('[');
            size_t pos2 = word.find(']');

            std::string prefix = "";
            std::string suffix = "";

            if (pos1 != 0){
                prefix = word.substr(0, pos1);
            }
            if (word[pos2] != word.back()){
                suffix = word.substr(pos2 + 1);
            }
            
            for (size_t i = pos1 + 1; i < pos2; i++){
                std::string w = prefix + std::string(1, word[i])
                + suffix;
                if (w != beginWord && w != endWord){
                dict.push_back(dictWord(w));
                }
            }
        }

        void processSwap(const std::string& word){  
            size_t pos = word.find('!');
        
            if (pos < 2 || word.size() < 3){
                return;
            }

            std::string prefix = "";
            std::string suffix = "";

            if (pos > 2){
                prefix = word.substr(0, pos - 2);
            }
            if (word[pos] != word.back()){
                suffix = word.substr(pos + 1);
            }

            //original first
            std::string swapped = prefix + std::string(1, word[pos - 2]) 
            + std::string(1, word[pos - 1]) + suffix;
            if (swapped != endWord && swapped != beginWord){
            dict.push_back(dictWord(swapped));
            }
            //then push back swapped
            swapped = prefix + std::string(1, word[pos - 1]) 
            + std::string(1, word[pos - 2]) + suffix;
            if (swapped != endWord && swapped != beginWord){
            dict.push_back(dictWord(swapped));
            }
        }

        void processDouble(const std::string& word){
            size_t pos = word.find('?');

            if(pos < 1 || word.size() < 2){
                return;
            }

            std::string prefix = "";
            std::string suffix = "";

            if (pos > 1){
                prefix = word.substr(0, pos - 1);
            }
            if (word[pos] != word.back()){
                suffix = word.substr(pos + 1);
            }

            //original first
            std::string doubled = prefix + std::string(1, word[pos - 1]) + suffix;
            if (doubled != endWord && doubled != beginWord){
            dict.push_back(dictWord(doubled));
            }
            //then push back doubled
            doubled = prefix + std::string(2, word[pos - 1]) + suffix;
            if (doubled != endWord && doubled != beginWord){
            dict.push_back(dictWord(doubled));
            }
        }



        void search(){
            sc.push_back(0);
            dict[0].discovered = true;
            num_words_that_entered_SC++;

        while (!sc.empty()){//large while loop

            if (stackMode){//stack
                current = sc.back();
                sc.pop_back();
            }else{//queue
                current = sc.front();
                sc.pop_front();
            }
        

            std::string current_word = dict[static_cast<size_t>(current)].word;
            //  std::cout << current_word << "current word" << '\n';
            //investigate all undiscovered words in the vector
            for (size_t i = 0; i < dict.size(); i++){
                            
                //RIGHT NOW ALL BEING DISCOVERED AFTER FIRST ITERATION 
                if ((!dict[i].discovered && similarTo(current_word, dict[i].word))){
                    sc.push_back(static_cast<int>(i));
                    num_words_that_entered_SC++;
                    dict[i].discovered = true;
                    dict[i].prev = current;
                    if (dict[1].discovered){
                        current = 1;
                        sc.clear();
                        return success_output();
                    }
                }
            }//end of investigate
            //  std::cout << '\n';
    }//end of large while loop
    //num_words_that_entered_SC--;
    std::cout << "No solution, " << num_words_that_entered_SC << " words discovered.";
}// end of search

        bool similarTo(const std::string &currWord,
         const std::string &potentialMorphWord){
            bool is_change_possible = false;
            bool is_length_possible = false;
            bool is_swap_possible = false;
            // std::cerr << currWord << '\n';
            // std::cerr << potentialMorphWord << '\n';


            if (changeMode){
                is_change_possible = changePossible(currWord, potentialMorphWord);
            }

            if (lengthMode){
                is_length_possible = lengthPossible(currWord, potentialMorphWord);
            }

            if (swapMode){
                is_swap_possible = swapPossible(currWord, potentialMorphWord);
            }

            // std::cerr << is_change_possible;
            // std::cerr << is_length_possible;
            // std::cerr << is_swap_possible << '\n';


            return (is_change_possible || is_length_possible || is_swap_possible);
        }

    

    bool changePossible(const std::string &currWord,
         const std::string &potentialMorphWord){

        if (currWord.length() == potentialMorphWord.length()){
            int count = 0;

            for (size_t i = 0; i < currWord.size(); ++i){ 
                if (currWord[i] != potentialMorphWord[i]){
                    count++;
                }
            } // end of loop checking for how many different chars
                    
            if (count == 1){
                return true;
            }
                
        }
    return false;
    }//end of changePossible
    
    //NEEDS TO BE CORRECTED
    bool lengthPossible(const std::string &currWord,
         const std::string &potentialMorphWord){
            int length_difference = std::abs(static_cast<int>(currWord.size()) - static_cast<int>(potentialMorphWord.size()));

    // If the strings are more or less than 1 character apart in length, return false
            if (length_difference != 1) {
                return false;
            }

            const std::string& longer = currWord.size() > potentialMorphWord.size() ? currWord : potentialMorphWord;
            const std::string& shorter = currWord.size() > potentialMorphWord.size() ? potentialMorphWord : currWord;

            bool foundDifference = false;
            for (size_t i = 0, j = 0; i < longer.size() && j < shorter.size();) {
                if (longer[i] != shorter[j]) {
                    if (foundDifference) { return false; }
                    foundDifference = true;
                i++; // Skip a character in the longer string
        } else {
                i++;
                j++;
        }
    }

    return true;
}// end of length possible

         bool swapPossible(const std::string &currWord,
         const std::string &potentialMorphWord){

            if (currWord.size() != potentialMorphWord.size()){
                return false;
            }
            size_t words_size = currWord.size();
            
            size_t firstDiscrepancy = words_size, secondDiscrepancy = words_size;

    for (size_t i = 0; i < words_size; ++i) {
        if (currWord[i] != potentialMorphWord[i]) {
            if (firstDiscrepancy == words_size) {
                firstDiscrepancy = i;
            } else if (secondDiscrepancy == words_size) {
                secondDiscrepancy = i;
            } else {
                return false;
            }
        }
    }

    return secondDiscrepancy != words_size && 
           currWord[firstDiscrepancy] == potentialMorphWord[secondDiscrepancy] && 
           currWord[secondDiscrepancy] == potentialMorphWord[firstDiscrepancy] &&
           std::abs(static_cast<int>(firstDiscrepancy) - static_cast<int>(secondDiscrepancy)) == 1;
         }//end of swap


//OUTPUT
         void success_output(){
            std::vector <int> ov;// vector of size numWords, output vector (implemented as a stack)
            ov.reserve(num_words_that_entered_SC);

            ov.push_back(current); // push back the end word index
            
            while (current != -1){
                ov.push_back(dict[static_cast<size_t>(current)].prev); // push back the index of previous word
                current = dict[static_cast<size_t>(current)].prev; // make the current index the previous index
            }

            ov.pop_back(); // pop -1 out of the vector 

            std::cout << "Words in morph: " << static_cast<int>(ov.size()) << "\n"; // UP UNTIL HERE THE W & M ARE THE SAME

            if (outputFormat == "M"){
                modification_output_mode_output(ov);
            }else{
                word_output_mode_output(ov);
            }
         }

         void word_output_mode_output(std::vector<int> &ov){
            while (!ov.empty()){
                std::cout << dict[static_cast<size_t>(ov.back())].word;
                ov.pop_back();
                if (!ov.empty()){std::cout << '\n';}
            }
         }

         void modification_output_mode_output(std::vector<int> &ov){
            // cout first word
            std::cout << dict[static_cast<size_t>(ov[ov.size()-1])].word << '\n';

            std::string original_word;
            std::string changed_word;

            //while
    while (ov.size() > 1){
            char type_of_change = '/';
            int index_of_change = -1;
            char letter_changed = '/';

            size_t ovSize = ov.size();
            original_word = dict[static_cast<size_t>(ov[ovSize - 1])].word;
            changed_word = dict[static_cast<size_t>(ov[ovSize - 2])].word;
            
            //insert case
            if (changed_word.size() > original_word.size()){
                type_of_change = 'i';
                    for (size_t i = 0; i < original_word.size(); i++){
                        if (original_word[i] != changed_word[i]){
                            index_of_change = static_cast<int>(i);
                            letter_changed = changed_word[i];
                            break; // breaks inner for
                        }
                    }
                    if (index_of_change == -1){
                    index_of_change = static_cast<int>(changed_word.size() - 1);
                    letter_changed = changed_word[static_cast<size_t>(index_of_change)];
                    }
            }

            //delete case
            else if (changed_word.size() < original_word.size()){
                type_of_change = 'd';
                    for (size_t i = 0; i < changed_word.size(); i++){
                        if (original_word[i] != changed_word[i]){
                            index_of_change = static_cast<int>(i);
                            //letter_changed = changed_word[i];
                            break; // breaks inner for
                        }
                    }
                    if (index_of_change == -1){
                        index_of_change = static_cast<int>(original_word.size()-1);
                    }
            }

            else {
                int numDiffs = 0;
                for (size_t i = 0; i < changed_word.size(); i++){
                        if (original_word[i] != changed_word[i]){
                            if (numDiffs == 1){
                                numDiffs++;
                                break;
                            }
                            if (numDiffs == 0){ 
                                index_of_change = static_cast<int>(i);
                                letter_changed = changed_word[i]; 
                                numDiffs++;
                            }
                        }
                }
                if (numDiffs == 1){
                    type_of_change = 'c';   
                } else{
                    type_of_change = 's';   
                    letter_changed = '/';
                }
            }
             //FINALLY OUTPUT
            std::cout << type_of_change << ',' << index_of_change;

            if (letter_changed != '/'){
                std::cout << "," << letter_changed;
            }
            if (ov.size() > 2){
                std::cout << '\n';
            }
        ov.pop_back();
    }
}
};

int main (int argc, char *argv[]){
    std::ios_base::sync_with_stdio(false);
    Letterman l;
    l.getOptions(argc, argv);
    l.readInput();
    l.search();
    return 0;
}
