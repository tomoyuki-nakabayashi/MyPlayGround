#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token identifiers
enum {
    TK_NUM = 256,  // Integer token
    TK_EOF,        // End of input
};


typedef struct {
    int ty;      // Token type
    int val;     // The value if ty is TK_NUM
    char *input; // Token strings (for error message)
} Token;

// Preserve the tokenizer result.
// Assumes tokens up to 100.
Token tokens[100];

enum {
    ND_NUM = 256  // Integer node
};

typedef struct Node {
    int op;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

// Prototype declarations
Node *expr();
void error(int);

void gen(Node *node) {
    if (node->op == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->op) {
        case '+':
            printf("  add rax, rdi\n");
            break;
        case '-':
            printf("  sub rax, rdi\n");
            break;
        case '*':
            printf("  mul rdi\n");
            break;
        case '/':
            printf("  mov rdx, 0\n");
            printf("  div rdi\n");
    }

    printf("  push rax\n");
}

Node *new_node(int op, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->op = ND_NUM;
    node->val = val;
    return node;
}

int32_t pos = 0;
Node *term() {
    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);
    if (tokens[pos].ty == '(') {
        pos++;
        Node *node = expr();
        if (tokens[pos].ty != ')') {
            fprintf(stderr, "Cloud not found `)`.");
            error(pos);
        }
        pos++;
        return node;
    }
    fprintf(stderr, "Unexpected token. Expect a number of '('.");
    error(pos);
}

Node *mul() {
    Node *lhs = term();
    if (tokens[pos].ty == TK_EOF)
        return lhs;
    if (tokens[pos].ty == '*') {
        pos++;
        return new_node('*', lhs, mul());
    }
    if (tokens[pos].ty == '/') {
        pos++;
        return new_node('/', lhs, mul());
    }

    return lhs;
    error(pos);
}

Node *expr() {
    Node *lhs = mul();

    if (tokens[pos].ty == TK_EOF)
        return lhs;

    if (tokens[pos].ty == '+') {
        pos++;
        return new_node('+', lhs, expr());
    }

    if (tokens[pos].ty == '-') {
        pos++;
        return new_node('-', lhs, expr());
    }

    return lhs;
    fprintf(stderr, "Unexpected token. Expect '+' or '-'.\n");
    error(pos);
}

// Split token from strings pointed by p.
void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

void error(int i) {
    fprintf(stderr, "Unexpected token: %s\n", tokens[i].input);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Ivalid arguments number.");
        return 1;
    }

    // Tokenize and parse.
    tokenize(argv[1]);
    Node* ast = expr();

    // Output the upper half of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Generate code traversing AST.
    gen(ast);

    // pop final result.
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}