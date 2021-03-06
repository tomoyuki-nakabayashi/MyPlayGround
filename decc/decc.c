#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// position of token.
int32_t pos = 0;

// token identifiers
enum {
    TK_NUM = 256,  // Integer token
    TK_IDENT,      // Identifier
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
    ND_NUM = 256, // Integer node
    ND_IDENT,     // Identifier node
};

typedef struct Node {
    int op;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;         // for ND_IDENT
} Node;

Node *code[100];

// Prototype declarations
Node *expr();
void error(int);

void gen_lval(Node *node) {
    if (node->op == ND_IDENT) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", ('z' - node->name + 1) * 8);
        printf("  push rax\n");
        return;
    }

    fprintf(stderr, "lvalue is not a variable\n");
    error(pos);
}

void gen(Node *node) {
    if (node->op == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    if (node->op == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if (node->op == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
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

Node *new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->op = ND_IDENT;
    node->name = name;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->op = ND_NUM;
    node->val = val;
    return node;
}

Node *term() {
    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);
    if (tokens[pos].ty == TK_IDENT)
        return new_node_ident(tokens[pos++].val);
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

Node *assign() {
    Node *lhs = expr();
    if (tokens[pos].ty == TK_EOF)
        return lhs;

    if (tokens[pos].ty == '=') {
        pos++;
        lhs = new_node('=', lhs, expr());
        if (tokens[pos].ty == ';') {
            pos++;
            return lhs;
        }
    }

    if (lhs->op == TK_NUM)
        return lhs;
}

void *program() {
    int n = 0;
    while (tokens[pos].ty != TK_EOF) {
        code[n] = assign();
        n++;
    }

    code[n] = NULL;  // NULL terminated.
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

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
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
    program();

    // Output the upper half of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // prolog
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    // Generate code traversing AST.
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        printf("  pop rax\n");
    }

    // epilog
    // pop final result.
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}