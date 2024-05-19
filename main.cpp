#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_SIZE 26
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

typedef struct {
    char phone_number[15];
    char email[100];
} ContactInfo;

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;  // Vrai si le noeud représente la fin d'un mot
    ContactInfo *info; // Information du contact
} TrieNode;

TrieNode* createNode(void) {
    TrieNode *Node = (TrieNode *)malloc(sizeof(TrieNode)); // Allocation de mémoire pour le noeud
    if (Node) {
        int i;
        Node->isEndOfWord = false;
        Node->info = NULL;
        for (i = 0; i < ALPHABET_SIZE; i++)
            Node->children[i] = NULL;
    }
    return Node;
}

void insert(TrieNode *root, const char *key, ContactInfo *info) {
    int level;
    int length = strlen(key);
    int index;
    TrieNode *pTrie = root;

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(key[level]));
        if (!pTrie->children[index])
            pTrie->children[index] = createNode();

        pTrie = pTrie->children[index];
    }

    pTrie->isEndOfWord = true;
    pTrie->info = (ContactInfo *)malloc(sizeof(ContactInfo));
    strcpy(pTrie->info->phone_number, info->phone_number);
    strcpy(pTrie->info->email, info->email);
}

bool search(TrieNode *root, const char *key, char *buffer) {
    int level;
    int length = strlen(key);
    int index;
    TrieNode *pTrie = root;

    buffer[0] = '\0'; // Initialiser le buffer

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(key[level]));

        if (!pTrie->children[index])
            return false;

        buffer[level] = key[level];
        pTrie = pTrie->children[index];
    }

    buffer[length] = '\0';

    if (pTrie != NULL && pTrie->isEndOfWord) {
        return true;
    }

    return false;
}

void displayContactsUtil(TrieNode *curNode, char prefix[], int length) {
    if (curNode->isEndOfWord) {
        prefix[length] = '\0';
        printf("Contact: %s, Phone: %s, Email: %s\n", prefix, curNode->info->phone_number, curNode->info->email);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (curNode->children[i]) {
            prefix[length] = 'a' + i;
            displayContactsUtil(curNode->children[i], prefix, length + 1);
        }
    }
}

void displayAllContacts(TrieNode *root) {
    char prefix[100];
    displayContactsUtil(root, prefix, 0);
}

void deleteContact(TrieNode *root, const char *key) {
    int level;
    int length = strlen(key);
    int index;
    TrieNode *pTrie = root;

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(key[level]));

        if (!pTrie->children[index])
            return;

        pTrie = pTrie->children[index];
    }

    if (pTrie != NULL && pTrie->isEndOfWord) {
        pTrie->isEndOfWord = false;
        free(pTrie->info);
    }
}

void deleteTrie(TrieNode *root) {
    if (!root) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i])
            deleteTrie(root->children[i]);
    }
    if (root->info) {
        free(root->info);
    }
    free(root);
}



// Fonction auxiliaire pour parcourir le Trie et écrire les contacts dans le fichier
void saveContactsUtil(TrieNode *curNode, char prefix[], int length, FILE *file) {
    if (curNode->isEndOfWord) {
        prefix[length] = '\0';
        fprintf(file, "%s,%s,%s\n", prefix, curNode->info->phone_number, curNode->info->email);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (curNode->children[i]) {
            prefix[length] = 'a' + i;
            saveContactsUtil(curNode->children[i], prefix, length + 1, file);
        }
    }
}

void saveContactsToFile(TrieNode *root, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    char prefix[100];
    saveContactsUtil(root, prefix, 0, file);

    fclose(file);
}

int main() {
    TrieNode *root = createNode();

    // Insertion de contacts
    ContactInfo john = {"0123456789", "johnwick@example.com"};
    insert(root, "johnwick", &john);

    ContactInfo mcclane = {"0987654321", "johnmcclane@example.com"};
    insert(root, "JohnMcclane", &mcclane);

    // Affichage de tous les contacts après l'insertion
    printf("\nAll contacts after insertion:\n");
    displayAllContacts(root);

    // Recherche de contacts
    printf("\nSearching for contacts...\n");
    char nameBuffer[100];
    if (search(root, "johnwick", nameBuffer)) {
        printf("Found: %s\n", nameBuffer);
    } else {
        printf("Contact '%s' not found.\n", "johnwick");
    }

    if (search(root, "JohnMcclane", nameBuffer)) {
        printf("Found: %s\n", nameBuffer);
    } else {
        printf("Contact '%s' not found.\n", "JohnMcclane");
    }

    // Sauvegarde des contacts dans un fichier CSV
    printf("\nSaving contacts to file...\n");
    saveContactsToFile(root, "annuaire.csv");
    if(ferror(stdout)) {
        perror("Error writing to file");
    } else {
        printf("Contacts saved to file 'annuaire'.\n");
    }

    // Suppression d'un contact
    deleteContact(root, "johnwick");
    printf("\nContact 'johnwick' deleted.\n");

    // Affichage de tous les contacts après la suppression
    printf("\nAll contacts after deletion:\n");
    displayAllContacts(root);

    // Recherche d'un contact qui a été supprimé
    printf("\nSearching for a deleted contact...\n");
    if (search(root, "johnwick", nameBuffer)) {
        printf("Found: %s\n", nameBuffer);
    } else {
        printf("Contact '%s' not found.\n", "johnwick");
    }

    // Suppression finale du Trie
//    deleteTrie(root);

    return 0;
}
