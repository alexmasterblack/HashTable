#include "pch.h"
#include <vector>
#include <unordered_map>

TEST(Allocator, A) {
	fefu::allocator<int> alloc;
	int* place = alloc.allocate();
	*place = 15;
	EXPECT_EQ(*place, 15);
}

TEST(Constructors, A) {
	fefu::hash_map<std::string, int> table(7);
	EXPECT_EQ(table.bucket_count(), 7);
	EXPECT_EQ(table.size(), 0);
	EXPECT_EQ(table.size(), 0);
	EXPECT_TRUE(table.empty());
}

TEST(Constructors, B) {
	fefu::hash_map<std::string, int> table;
	EXPECT_EQ(table.bucket_count(), 0);
	EXPECT_EQ(table.size(), 0);
	EXPECT_EQ(table.size(), 0);
	EXPECT_EQ(table.load_factor(), 0);
}

TEST(Constructors, C) {
	fefu::hash_map<std::string, int> table_one(10);
	table_one.insert(std::pair<std::string, int>("help", 4));
	table_one.insert(std::pair<const std::string, int>("death", 1));
	table_one.insert(std::pair<const std::string, int>("make", 8));
	table_one.insert(std::pair<const std::string, int>("peace", 6));
	table_one.insert(std::pair<const std::string, int>("death", 5));

	fefu::hash_map<std::string, int> table(table_one);
	EXPECT_TRUE(table.contains("death"));
	EXPECT_FALSE(table.contains("rest"));
	EXPECT_FALSE(table.contains("in"));
	EXPECT_TRUE(table.contains("peace"));
	EXPECT_TRUE(table.contains("help"));
}

TEST(Constructors, D) {
	std::vector<std::pair<std::string, int>> elements(9);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };
	fefu::hash_map<std::string, int> table(elements.begin(), elements.end());
	std::cout << table.contains("death");
}

TEST(Constructors, E) {
	fefu::hash_map<std::string, int> table_one(10);
	std::vector<std::pair<std::string, int>> elements(5);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };
	table_one.insert(elements.begin(), elements.end());
	fefu::hash_map<std::string, int> table = std::move(table_one);
}

TEST(Constructors, F) {
	fefu::hash_map<std::string, int> table({ {"death", 1}, {"rest", 23}, {"help", 3}, {"my", 11}, {"give", 11} });
	EXPECT_EQ(table.at("death"), 1);
	EXPECT_FALSE(table.cbegin() == table.cend());
}

TEST(Constructors, G) {
	std::vector<std::pair<std::string, int>> elements(9);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };
	fefu::hash_map<std::string, int> table(elements.begin(), elements.end(), 7);
	std::cout << table.contains("death");
}

TEST(Iterators, A) {
	fefu::hash_map<int, char> table;
	EXPECT_EQ(table.begin(), table.end());
	EXPECT_TRUE(table.cbegin() == table.cend());
}

TEST(Iterators, B) {
	fefu::hash_map<std::string, int> table(2);
	std::pair<std::string, int> element{ "yes", 1 };
	table.insert(element);
	EXPECT_TRUE(table.begin()->first == "yes");
	EXPECT_TRUE(table.begin()->second == 1);
}

TEST(Iterators, C) {
	fefu::hash_map<std::string, int> table({ {"death", 1}, {"rest", 23}, {"help", 3}, {"my", 11}, {"give", 11} });
	EXPECT_FALSE(table.cbegin() == table.cend());
}

TEST(Insert, A) {
	fefu::hash_map<std::string, int> table(10);
	std::vector<std::pair<std::string, int>> elements(5);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };

	table.insert(elements.begin(), elements.end());
	EXPECT_TRUE(table.contains("death"));
	EXPECT_TRUE(table.contains("rest"));
	EXPECT_TRUE(table.contains("in"));
	EXPECT_TRUE(table.contains("peace"));
	EXPECT_TRUE(table.contains("help"));
}

TEST(Insert, B) {
	fefu::hash_map<std::string, int> table(5);
	table.insert(std::pair<std::string, int>("help", 4));
	table.insert(std::pair<const std::string, int>("death", 1));
	table.insert(std::pair<const std::string, int>("make", 8));
	table.insert(std::pair<const std::string, int>("peace", 6));
	table.insert(std::pair<const std::string, int>("death", 5));


}
TEST(At, A) {
	fefu::hash_map<std::string, int> table(10);
	std::pair<std::string, int> element_one("death", 1);
	std::pair<std::string, int> element_two("rest", 2);
	table.insert(element_one);
	table.insert(element_two);

	EXPECT_EQ(table.at("death"), 1);
	EXPECT_EQ(table.at("rest"), 2);
	EXPECT_THROW(table.at("help_me"), std::out_of_range);

}

TEST(At, B) {
	//std::pair<const std::string, const int> element_one("death", 1);
	fefu::hash_map<std::string, int> table(7);
	std::vector<std::pair<std::string, int>> elements(5);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };
	//fefu::hash_map<std::string, int> table(elements.begin(), elements.end());

	table.insert(elements.begin(), elements.end());
	EXPECT_EQ(table.at("death"), 1);
	EXPECT_THROW(table.at("help_me"), std::out_of_range);

}

TEST(At, C) {
	std::vector<std::pair<std::string, int>> elements(9);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	elements[2] = { "in", 3 };
	elements[3] = { "peace", 4 };
	elements[4] = { "help", 5 };
	fefu::hash_map<std::string, int> table(elements.begin(), elements.end());
	EXPECT_TRUE(table.contains("death"));
}

TEST(Size, A) {
	fefu::hash_map<std::string, int> table(2);
	std::vector<std::pair<std::string, int>> elements(2);
	elements[0] = { "death", 1 };
	elements[1] = { "rest", 2 };
	//table.insert(elements.begin(), elements.end());
	//EXPECT_TRUE(table.max_size() == std::numeric_limits<size_t>::max());
}

TEST(Operators, A) {
	fefu::hash_map<int, int> table(10);
	table[6] = -120;
	table[2] = 12;
	EXPECT_TRUE(table[6] == -120);
	EXPECT_TRUE(table[2] == 12);
}

TEST(Operators, B) {
	fefu::hash_map<std::string, int> table(10);
	const std::string word = "please";
	const int number = 6;
	table[word] = number;
	EXPECT_TRUE(table["please"] == 6);
	EXPECT_TRUE(table.at("please") == 6);
}

TEST(Operators, C) {
	fefu::hash_map<std::string, int> table({ {"ab1", 1}, {"ab2", 2}, {"ab3", 3}, {"ab4", 4} });
	fefu::hash_map<std::string, int> table_one(table);
}

TEST(Bucket, A) {
	fefu::hash_map<std::string, int> table({ {"give", 11}, {"love", 41}, {"me", 3}, {"death", 41} });
	ASSERT_TRUE(table.bucket("give"));
	ASSERT_TRUE(table.bucket("love"));

	table.insert(std::pair<std::string, int>("help", 4));
	table.insert(std::pair<std::string, int>("death", 1));
	table.insert(std::pair<std::string, int>("make", 8));
	table.insert(std::pair<std::string, int>("peace", 6));

	ASSERT_TRUE(table.bucket("help"));
	ASSERT_TRUE(table.bucket("death"));
}
