#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


struct user {   // user list 
	char name[20];
	int *point_list;
	float av_point;
};

struct recom {  // struct for similarity values and indexes
	int index;
	float sim;
};

struct recom* calculate_sim(struct user*,int , int ,int ,int ,int );  // calculate similarity for all U's to selected(input) NU from user
// and returns a similarty array of all U's similarity values and their indexes
struct recom* calculate_pred(struct user* ,int ,int ,struct recom* ,int,int ); // calculates pred value for all unred books for selected NU and returns a 
// struct recom array of books and their predicted given points array

int main() {
	char line[1000];  // variable for storing lines from file
	FILE* stream ;  // file pointer for file reading
	char *token;   // char pointer to use with strtok
	const char *sec; // char pointer to use with strtok
	int flag = 0; // variable to understand which line we are reading from file
	int flag_value,given_point = 0;  //  varibles to store book numbers and given points from users
	int size = 0;  // varible to understand how many users in csv
	int size_value; // variable to understand how many users in csv
	char **book_list; // book names
	int username_flag = 0 ; // flag to read username first in lines
	int read_book = 0,total_point=0;  // varibles to calculate number of read books and total given points
	int index = 0;   // index values for file reading
	struct user* user_list; // user struct array for user infos, names their av given points 
	struct recom* similar_users; // to store k similar user
	char recomm[20]; // which user to recommend book input from user
	int user_n = 0;  // total U numbers stored after reading from file
	int new_user = 0;  // total NU numbers stored after reading from file
	int k;  // k value entered by user
	int i,j,unread_books=0;  // loop variables and variable that store number of unread books for selected NU
	float gre;  // to get max K values
	struct recom* sim;  // similarity values of all users to selected NU array
	struct recom* recommended_books; // recom struct array that store recommended books after selection 
	int wanted_index = 0;  // to get wanted index with selected(input) NU from user
	int loop_flag= 1;    // flag to use select mod or exit program options 
	int check_value = -2; // flag for understanding valid inputs for K and NU values

	printf("Book Recommedation -> 1\nExit Program -> Any value other than 1\n");
	printf("Select Mode : ");
	scanf("%d",&loop_flag);
	printf("\n");

	while(loop_flag == 1) {

		stream = fopen("RecomendationDataSet.csv", "r");

		while(fgets(line,1000, stream)) {
			token = strtok(line,";");
			if(token[0] == 'U')
				user_n++;
			else if(token[0] == 'N')
				new_user++;
			size++;
			while( token != NULL ) {
				if(size == 1)
					flag++;
				token = strtok(NULL, ";");
			}
		}
		user_n--;
		free(token);

		flag_value = flag-1;
		size_value = size-1;
		
		printf("Enter the username for book recommendation : ");
		scanf("%s",recomm);
		printf("Enter the k(similar user) number: ");
		scanf("%d",&k);

		if(k > user_n || k <= 0)
			printf("\nK value should be between 0 and %d\n\n",user_n);
		else {
			similar_users = (struct recom*)malloc(k*sizeof(struct recom));

			book_list = (char **)malloc((flag_value) * sizeof(char*));

			for(i=0 ; i<flag-1 ; i++)
				book_list[i] = (char *)malloc(30 * sizeof(char));

			user_list = (struct user*)malloc((size_value) * sizeof(struct user));

			for(i=0 ; i<size_value; i++)
				user_list[i].point_list = (int *)malloc(flag_value * sizeof(int));

			fclose(stream);
			stream = fopen("RecomendationDataSet.csv","r");

			flag=-1;
			size= 0;
			while(fgets(line,1000, stream)) {
				if(size==0)
					line[strlen(line)-1] = '\0';
				sec = strtok(line,";");
				while(sec != NULL) {
					if(sec != NULL && flag != -1 && size == 0) {
						sec = strtok(NULL, ";");

						if(sec != NULL)
							strcpy(book_list[flag++],sec);
					}
					if(flag == -1)
						flag++;
					if(size > 0) {
						if(sec != NULL) {
							if(username_flag == 0) {
								strcpy(user_list[size-1].name,sec);
								username_flag = 1 ;
							} else {
								given_point = *sec - '0';
								if(given_point > 0) {
									read_book++;
									total_point += given_point;
								}
								user_list[size-1].point_list[index] = *sec - '0';
								index++;
							}
						}
						sec = strtok(NULL, ";");
					}
				}

				if(size > 0)
					user_list[size-1].av_point = (float)total_point / (float)read_book;
				read_book = 0;
				total_point = 0;
				given_point = 0;
				username_flag = 0;
				index = 0;
				flag = 0;
				size++;
			}
			read_book = 0;
			total_point = 0;

			for(i=user_n; i<user_n+new_user; i++)
				if(strcmp(recomm,user_list[i].name) == 0)
					check_value = 0;
			if(check_value == -2)
				printf("\n%s is not a new user\n\n",recomm);
			else {
				for(i=user_n; i<user_n+new_user ; i++)
					if(strcmp(recomm,user_list[i].name) == 0)
						wanted_index = i;

				sim = calculate_sim(user_list,user_n,new_user,k,wanted_index,flag_value);

				for(j=0; j<k; j++) {           // instead of sorting, max K values from array selected and shifted to end of the array
					gre = sim[0].sim;
					for(i=0; i<user_n-1 ; i++) {
						if(sim[i].sim > sim[i+1].sim) {
							gre = sim[i+1].sim;
							sim[i+1].sim = sim[i].sim;
							sim[i].sim = gre;
							gre = sim[i+1].index;
							sim[i+1].index = sim[i].index;
							sim[i].index = gre;
						}
					}
				}
				for(j=0; j<k ; j++) {
					similar_users[j].index = sim[user_n-1-j].index;
					similar_users[j].sim = sim[user_n-1-j].sim;
				}

				for(i=0 ; i<flag_value ; i++)
					if(user_list[wanted_index].point_list[i] == 0)
						unread_books++;

				printf("------------------------------------\n");
				printf("Closest users to %s user(k=%d) and calculated pearson correlation coefficient\n",recomm,k);
				printf("<Username , similarities>\n");
				for(i=0; i<k; i++)
					printf("%d.  '%s'\t\t%f \n",i+1,user_list[similar_users[i].index].name,similar_users[i].sim);

				printf("\n\n");
				recommended_books = calculate_pred(user_list,wanted_index,flag_value,similar_users,k,user_n);

				printf("Calculated like rates for the books that has not been read by %s user \n",recomm);

				for(i=0; i<unread_books; i++)
					printf("%d. '%s'  %f \n",i+1,book_list[recommended_books[i].index], recommended_books[i].sim);

				j=0;
				gre = recommended_books[0].sim;
				for(i=0; i<unread_books; i++) {
					if(recommended_books[i].sim > gre) {
						j=i;
						gre = recommended_books[i].sim;
					}
				}
				printf("\nFinal Recommendation : %s\n",book_list[recommended_books[j].index]);

				printf("\n\nBook Recommedation -> 1\nExit Program -> Any value other than 1\n");
				printf("Select Mode : ");
				scanf("%d",&loop_flag);
				printf("\n");
				free(token);
				free(recommended_books);
				free(sim);
				unread_books = 0;
			}
			index = 0;			
			total_point = 0;
			given_point = 0;
			username_flag = 0;
			read_book = 0;
			fclose(stream);
			free(similar_users);
			free(user_list);
			free(book_list);
			flag = 0;
		}
		user_n = 0;
		new_user = 0;
		size = 0;
		flag = 0;
		fclose(stream);
		check_value = -2;
	}
	return 0;
}

struct recom* calculate_sim(struct user* user_list,int user_n , int new_user ,int k ,int wanted_index,int book_num) { // calculate similarties for all Users with input new user
	int i,j,read_book=0 , total_point_a= 0;
	int total_point_b= 0;
	struct recom* sim = (struct recom*)malloc(user_n*sizeof(struct recom));
	float num = 0;
	float denom = 0;
	float first= 0;
	float second = 0;
	float av_a= 0;
	float av_b = 0;
	float firstmin, secmin , thirdmin;
	for(i=0 ; i<user_n ; i++) {
		for(j=0 ; j<book_num; j++)
			if(user_list[wanted_index].point_list[j] > 0 && user_list[i].point_list[j] > 0) {
				read_book++;
				total_point_a += user_list[i].point_list[j];
				total_point_b += user_list[wanted_index].point_list[j];
			}
		av_a = (float)total_point_a / (float) read_book;
		av_b = (float)total_point_b / (float) read_book;
		read_book = 0;
		total_point_a = 0;
		total_point_b = 0;
		for(j=0 ; j<book_num ; j++) {
			if(user_list[wanted_index].point_list[j] > 0 && user_list[i].point_list[j] > 0) {
				num += ((user_list[i].point_list[j] - av_a )* (user_list[wanted_index].point_list[j] -av_b));
				first += pow((user_list[i].point_list[j] - av_a ),2);
				second += pow(user_list[wanted_index].point_list[j] -av_b,2);
			}
		}
		denom = sqrt(first) * sqrt(second);
		sim[i].sim = num / denom;
		sim[i].index = i;
		num = 0;
		first = 0;
		second = 0;

	}
	return sim;
}

struct recom* calculate_pred( struct user* user_list,int wanted_index,int book_num,struct recom* sim_user,int k,int user_n) {
	struct recom* books;
	int i,j;
	int unread_books=0;
	float total_num=0;
	float total_denom = 0;
	float result;
	for(i=0 ; i<book_num ; i++)
		if(user_list[wanted_index].point_list[i] == 0)
			unread_books++;
	books = (struct recom*)malloc(unread_books*sizeof(struct recom));

	j=0;
	for(i=0 ; i<book_num ; i++) {
		if(user_list[wanted_index].point_list[i] == 0) {
			books[j].index = i;
			j++;
		}

	}
	for(i=0; i<unread_books; i++) {
		for(j=0; j<k; j++) {
			total_num += (sim_user[j].sim * (user_list[sim_user[j].index].point_list[books[i].index] - user_list[sim_user[j].index].av_point));
			total_denom += sim_user[j].sim;
		}
		result = user_list[wanted_index].av_point + (total_num / total_denom);
		books[i].sim = result;
		total_num = 0;
		total_denom = 0;
	}
	return books;
}
