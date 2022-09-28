/**
 ** SixDegrees.cpp
 **
 ** CS 15 Proj2: Six Degrees of Collaboration
 **
 ** Purpose: The implementation of the Six Degrees class. It helps to achieve 
 ** all the required functionalities of the program, including the three 
 ** traversals, print and quit commands, and error messages for client 
 ** interaction. The function is based on a command loop that calls different 
 ** functions.
 **    
 ** Author: David Chen
 ** Created on: July 31, 2022
 **
 **/

#include <iostream>
#include <vector>
#include "Artist.h"
#include "CollabGraph.h"
#include "SixDegrees.h"
#include <queue>

using namespace std;

/* read_file
 * Purpose: Populate the graph by creating a vector of artists aquired from 
 * the istream input_file using getline
 * Parameters: An istream reference
 * Returns: None
 */
void SixDegrees::read_file(istream &input_file) 
{
    // store information about the artist
    vector<Artist> artists;
    string temp;

    // Store the first line of strings as the artist's name, and the rest of 
    // the string (until an astrid is reached) as their songs
    while (getline(input_file, temp)) {
        Artist temp_artist(temp);
        getline(input_file, temp);
        while (temp != "*") {
            temp_artist.add_song(temp);
            getline(input_file, temp);
        }
        // push each artist into the vector
        artists.push_back(temp_artist);
    }
    graph.populate(artists);
}

/* read_cmd
 * Purpose: Read in commands from an istream input and activate corresponding
 * functions to execude the command
 * Parameters: An istream reference and an ostream reference
 * Returns: None
 */
void SixDegrees::read_cmd(istream &input, ostream &output) 
{
    // a string that stores the input cmd
    string input_cmd;

    // execute each function and message (or quit the program) given the cmd
    while (getline(input, input_cmd)) {
        if (input_cmd == "quit") {
            return;
        } else if (input_cmd == "print"){
            graph.print_graph(output);
        } else if (input_cmd == "bfs") {
            handle_bfs(input, output);
        } else if (input_cmd == "dfs") {
            handle_dfs(input, output);
        } else if (input_cmd == "not") {
            handle_not(input, output);
        } else {
            output << input_cmd << " is not a command. Please try again." 
                   << endl;
        }
    }
}

/* handle_bfs
 * Purpose: Initiate the bfs command given the input; validate the input to 
 * determine whether to proceed or not
 * Parameters: An istream reference and an ostream reference
 * Returns: None
 */
void SixDegrees::handle_bfs(istream &input, ostream &output)
{
    // clear the graph to prepare for a new round of traversal
    graph.clear_metadata();
    
    // create two artists based on the given input
    string artist_name1;
    getline(input, artist_name1);
    Artist artist1(artist_name1);
    string artist_name2;
    getline(input, artist_name2);
    Artist artist2(artist_name2);
    
    // if the two artists are vertices of the graph, proceed
    if (graph.is_vertex(artist1) and graph.is_vertex(artist2)) {
        real_bfs(artist1, artist2, output);
    } else {
        if (not graph.is_vertex(artist1)) {
            not_vertex(artist1, output);
        }
        if (not graph.is_vertex(artist2)) {
            not_vertex(artist2, output);
        }
    }
}

/* real_bfs
 * Purpose: final stage of bfs; perform the traversal and stop when the 
 * destination is reached
 * Parameters: two artist references and an ostream reference
 * Returns: None
 */
void SixDegrees::real_bfs(const Artist &a1, const Artist &a2, ostream &output)
{
    // create a vector for neighbors and a queue for artist traversal
    vector<Artist> neighbors; 
    queue<Artist> curr;
    // mark the start vertex and push it to the queue
    graph.mark_vertex(a1);
    curr.push(a1);

    while (not curr.empty()) {
        neighbors = graph.get_vertex_neighbors(curr.front());
        for (size_t i = 0; i < neighbors.size(); i++) {
            if (not graph.is_marked(neighbors.at(i))) {
                graph.set_predecessor(neighbors.at(i), curr.front());
                graph.mark_vertex(neighbors.at(i));
                // push the neighbor onto the queue as the next vertex
                // to be processed (FIFO)
                curr.push(neighbors.at(i));
            }
            // stop the loop if the destination is reached since further 
            // search would be trivial
            if (neighbors.at(i) == a2) {
                break;
            }
        }
        curr.pop();
    }

    print_path(a1, a2, output);
}

/* handle_dfs
 * Purpose: Initiate the dfs command given the input; validate the input to 
 * determine whether to proceed or not
 * Parameters: An istream reference and an ostream reference
 * Returns: None
 */
void SixDegrees::handle_dfs(istream &input, ostream &output)
{
    // clear the graph to prepare for a new round of traversal
    graph.clear_metadata();

    // create two artists based on the given input
    string artist_name1;
    getline(input, artist_name1);
    Artist artist1(artist_name1);
    string artist_name2;
    getline(input, artist_name2);
    Artist artist2(artist_name2);
    
    // if the two artists are vertices of the graph, proceed
    if (graph.is_vertex(artist1) and graph.is_vertex(artist2)) {
        real_dfs(artist1, artist2, output);
    } else {
        if (not graph.is_vertex(artist1)) {
            not_vertex(artist1, output);
        }
        if (not graph.is_vertex(artist2)) {
            not_vertex(artist2, output);
        }
    }
}

/* real_dfs
 * Purpose: intermediate stage of dfs; calls a helper function to perform 
 * a recursive bfs traversal
 * Parameters: two artist references and an ostream reference
 * Returns: None
 */
void SixDegrees::real_dfs(const Artist &a1, const Artist &a2, ostream &output)
{
    recursive_dfs(a1, a2);
    print_path(a1, a2, output);
}

/* recursive_dfs
 * Purpose: final stage of bfs; recursively call the function to traverse 
 * to the end of each neighbor before moving to the next; stop when the 
 * destination is reached
 * Parameters: two artist references
 * Returns: None
 */
void SixDegrees::recursive_dfs(const Artist &a1, const Artist &a2)
{
    vector<Artist> neighbors;
    graph.mark_vertex(a1);
    neighbors = graph.get_vertex_neighbors(a1);

    for (size_t i = 0; i < neighbors.size(); i++) {
        if (not graph.is_marked(neighbors.at(i))) {
            graph.set_predecessor(neighbors.at(i), a1);
            // unlike a bfs traversal, dfs traversals would finish one entire 
            // route before moving onto the next
            if (neighbors.at(i) != a2) {
                recursive_dfs(neighbors.at(i), a2);
            } else {
                // stop the traversal once the destination is reached
                break;
            }
        }
    }
}

/* handle_dfs
 * Purpose: Initiate the not command given the input; validate the input to 
 * determine whether to proceed or not
 * Parameters: An istream reference and an ostream reference
 * Returns: None
 */
void SixDegrees::handle_not(std::istream &input, std::ostream &output)
{
    // clear the graph to prepare for a new round of traversal
    graph.clear_metadata();
    
    // create two artists based on the given input
    string artist_name1;
    getline(input, artist_name1);
    Artist artist1(artist_name1);
    string artist_name2;
    getline(input, artist_name2);
    Artist artist2(artist_name2);
    
    // create a vector of artists for a list of exclusions
    vector<Artist> excluded_artists;
    string excluded;
    getline(input, excluded);
    while (excluded != "*" and not input.eof()) {
        Artist temp(excluded);
        excluded_artists.push_back(temp);
        getline(input, excluded);
    }
    
    //proceed to the next stage of not
    check_not(artist1, artist2, excluded_artists, output);
}

/* check_not
 * Purpose: Intermediate stage of not that checks whether the inputted artists 
 * and vector of artists are valid vertices on the graph. 
 * Parameters: two artist references, one vector of artists and an ostream
 * reference
 * Returns: None
 */
void SixDegrees::check_not(const Artist &a1, const Artist &a2, 
                           vector<Artist> &excluded, ostream &output)
{
    bool proceed = false;
    if (graph.is_vertex(a1) and graph.is_vertex(a2)) {
        proceed = true;
    } else {
        if (not graph.is_vertex(a1)) {
            not_vertex(a1, output);
        }
        if (not graph.is_vertex(a2)) {
            not_vertex(a2, output);
        }
    }

    int count = 0;
    for (size_t i = 0; i < excluded.size(); i++) {
        if (not graph.is_vertex(excluded.at(i))) {
            count++;
            not_vertex(excluded.at(i), output);
        }
    }
    
    // only proceed if both the inputted artists and the excluded list are 
    // all valid vertices in the graph
    if (proceed and (count == 0)) {
        real_not(a1, a2, excluded, output);
    }
}

/* real_not
 * Purpose: Final stage of not that performs a bfs traversal that rules out 
 * all the excluded artists
 * Parameters: two artist references, one vector of artists and an ostream
 * reference
 * Returns: None
 */
void SixDegrees::real_not(const Artist &a1, const Artist &a2, 
                           vector<Artist> &excluded, ostream &output)
{
    // mark all the excluded vertices so they don't get included in the 
    // traversal
    while (not excluded.empty()) {
        graph.mark_vertex(excluded.back());
        excluded.pop_back();
    }

    // if the start or end of the traversal is excluded, print an empty path
    if (graph.is_marked(a1) or graph.is_marked(a2)) {
        print_path(a1, a2, output);
    } else {
        real_bfs(a1, a2, output);
    }
}

/* print_path
 * Purpose: A helper function that helps each traversal
 * Parameters: two artist references and an ostream reference
 * Returns: None
 */
void SixDegrees::print_path(const Artist &a1, const Artist &a2, 
                            std::ostream &output)
{
    // store the path into a stack of artists
    stack<Artist> path = graph.report_path(a1, a2);
   
   // print message if path is empty
    if (path.empty()) {
        output << "A path does not exist between \"" << a1.get_name() 
               << "\" and \"" << a2.get_name() << "\"." << endl;
    } else {
        // if the path is not empty, print itself, its next member and their 
        // edge
        while (path.size() > 1) {
            Artist temp = path.top();
            path.pop();
            string song = graph.get_edge(temp, path.top());
            output << "\"" << temp.get_name() << "\" collaborated with \""
                   << path.top().get_name() << "\" in " << "\"" << song 
                   << "\"." << endl;
        }
        output << "***" << endl;
    }
}

/* not_vertex
 * Purpose: A helper function that prints a message when an artist is not a 
 * valid vertex in the graph
 * Parameters: an artist reference and an ostream reference
 * Returns: None
 */
void SixDegrees::not_vertex(const Artist &a, ostream &output)
{
    output << "\"" << a.get_name() <<"\" was not found in the " << "dataset :("
           << endl;
}

