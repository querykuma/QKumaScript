#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "mylib.h"
#include <assert.h>

static char temp[1024];

_Noreturn void run_exit(Node *node, char *msg)
{
    if (msg)
    {
        fprintf(stderr, "error at line %llu: %s\n", node->line_num, msg);
    }
    else
    {
        fprintf(stderr, "error at line %llu\n", node->line_num);
    }
    exit(1);
}
Node *make_node(Node_Type type, Node *cond, Node *left, Node *right, Node *body, int64_t ivalue, char *svalue, int64_t line_num, Env *env)
{
    Node *node = (Node *)my_malloc_node(sizeof(Node));

    node->type = type;
    node->cond = cond;
    node->left = left;
    node->right = right;
    node->body = body;
    node->ivalue = ivalue;
    node->svalue = svalue;
    node->line_num = line_num;
    node->env = env;
    node->parent = NULL;

    node->return_value = 0;
    node->after_run = 0;
    node->env_var = 0;
    node->freed = 0;
    return node;
}
Node *make_node_num(int64_t ivalue)
{
    Node *node = make_node(Num_Node, NULL, NULL, NULL, NULL, ivalue, NULL, 0, NULL);
    node->after_run = 1;
    return node;
}
Node *make_node_closure(Node *node_in, Env *env)
{
    Node *node = make_node(Closure_Node, NULL, NULL, NULL, node_in, 0, NULL, 0, env);
    node->after_run = 1;
    return node;
}
Node *get_var(Env *env, char *key)
{
    Node *node = cpp_map_get(map_stringkey_kind, env->map_void, key);
    while (!node && env->parent)
    {
        env = env->parent;
        node = cpp_map_get(map_stringkey_kind, env->map_void, key);
    }

    if (!node)
    {
        return NULL;
    }

    debug_check_map_node(node);

    return node;
}
void set_parent_node(Node *node, Node *parent)
{
    // nodeの親にparentをセットする。
    if (node->parent)
    {
        return;
    }
    if (!parent)
    {
        return;
    }
    node->parent = parent;
}
void set_parent_nodes(Node *node)
{
    if (!node)
    {
        return;
    }
    if (node->cond)
    {
        set_parent_node(node->cond, node);
        set_parent_nodes(node->cond);
    }
    if (node->left)
    {
        set_parent_node(node->left, node);
        set_parent_nodes(node->left);
    }
    if (node->right)
    {
        set_parent_node(node->right, node);
        set_parent_nodes(node->right);
    }
    if (node->body)
    {
        set_parent_node(node->body, node);
        set_parent_nodes(node->body);
    }
}
void assign_node_nonvar(Env *env, char *key, Node *node_in)
{
    // nonvarとはたとえば「x = 1;」のときのことでparentをたどって変数を探してアサインする。
    // varとはたとえば「var x = 1;」のときのことで、そのenvに変数にアサインするのでparentをたどらない。
    Node *node = cpp_map_get(map_stringkey_kind, env->map_void, key);
    while (!node && env->parent)
    {
        env = env->parent;
        node = cpp_map_get(map_stringkey_kind, env->map_void, key);
    }

    if (!node)
    {
        sprintf(temp, "nonvar %s not found", key);
        run_exit(node_in, temp);
    }

    cpp_map_set(map_stringkey_kind, env->map_void, key, node_in);
}
void call_assign_parameters_recursive(Env *env, Node *callee, Node *caller)
{
    Node *n1;
    if (!callee)
    {
        return;
    }
    if (!caller)
    {
        return;
    }

    if (callee->type == Parameter_Node)
    {
        n1 = run(caller->body, env);

        cpp_map_set(map_stringkey_kind, env->map_void, callee->svalue, n1);
        n1->env_var = 1;
    }
    else if (callee->type == Parameter_List_Node)
    {
        Node *callee_left = callee->left;
        Node *callee_right = callee->right;
        Node *caller_left = caller->left;
        Node *caller_right = caller->right;
        call_assign_parameters_recursive(env, callee_left, caller_left);
        call_assign_parameters_recursive(env, callee_right, caller_right);
    }
    else
    {
        run_exit(caller, "unexpected callee->type: call_assign_parameters_recursive");
    }
}
void call_assign_parameters(Env *env, Node *callee, Node *caller)
{
    // Function_Nodeで来ることは保証されているがダブルチェック。
    if (callee->type != Function_Node)
    {
        sprintf(temp, "call_assign_parameters Function_Node");
        run_exit(caller, temp);
    }

    call_assign_parameters_recursive(env, callee->left, caller->left);
}
void print_node(Node *node, Env *env)
{
    char *s = node_tostring(node, env, 0);
    printf("%s\n", s);
}
int8_t is_function_parameter(Node *node)
{
    // Function_Node下のparameterかCall_Node下のparameterかを判定する。
    Node *parent;

    while (node)
    {
        if (!(node->type == Parameter_Node || node->type == Parameter_List_Node))
        {
            run_exit(node, "system error: Parameter_Node nor Parameter_List_Node");
        }

        parent = node->parent;
        if (parent->type == Function_Node)
        {
            return 1;
        }
        else if (parent->type == Call_Node)
        {
            return 0;
        }

        node = node->parent;
    }
    run_exit(node, "system error: Parameter_Node nor Parameter_List_Node2");
}
char *node_tostring(Node *node, Env *env, int depth)
{
    if (node == NULL)
        return NULL;

    char *ret = NULL;
    char *cdepth = space_repeat(depth);
    char *cleft = node_tostring(node->left, env, depth + 1);
    char *cright = node_tostring(node->right, env, depth + 1);
    char *cbody = node_tostring(node->body, env, depth + 1);
    char *ccond = node_tostring(node->cond, env, depth + 1);

    switch (node->type)
    {
    case Stmt_List_Node:
        if (cleft)
        {
            ret = my_strcat(cdepth, "(Stmt_List\n", cleft, cright, cdepth, ")\n", VEND);
        }
        else
        {
            ret = my_strcat(cdepth, "(StmtList(no left)\n", cright, cdepth, ")\n", VEND);
        }
        break;
    case Print_Val_Node:
        ret = my_strcat(cdepth, "(Print Val\n", cleft, cdepth, ")\n", VEND);
        break;
    case Println_Node:
        ret = my_strcat(cdepth, "(Println)\n", VEND);
        break;
    case If_Node:
        ret = my_strcat(cdepth, "(If\n", ccond, cleft, cright, cdepth, ")\n", VEND);
        break;
    case While_Node:
        ret = my_strcat(cdepth, "(While\n", ccond, cbody, cdepth, ")\n", VEND);
        break;
    case For_Node:
        ret = my_strcat(cdepth, "(For\n", cleft, ccond, cright, cbody, cdepth, ")\n", VEND);
        break;
    case Assign_Node:
        sprintf(temp, "(Assign [%s]\n", node->svalue);
        ret = my_strcat(cdepth, temp, cleft, cdepth, ")\n", VEND);
        break;
    case Assign_Var_Node:
        sprintf(temp, "(Assign Var [%s]\n", node->svalue);
        ret = my_strcat(cdepth, temp, cleft, cdepth, ")\n", VEND);
        break;
    case Function_Node:
        ret = my_strcat(cdepth, "(Function\n", cleft, cbody, cdepth, ")\n", VEND);
        break;
    case Call_Node:
        ret = my_strcat(cdepth, "(Call \"", node->svalue, "\"\n", cleft, cdepth, ")\n", VEND);
        break;
    case Return_Node:
        ret = my_strcat(cdepth, "(Return\n", cbody, cdepth, ")\n", VEND);
        break;
    case Parameter_List_Node:
        ret = my_strcat(cdepth, "(Parameter List\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Parameter_Node:
        if (is_function_parameter(node))
        {
            // Function_Node下のparameter
            ret = my_strcat(cdepth, "(Parameter ", node->svalue, ")\n", VEND);
        }
        else
        {
            // Call_Node下のparameter
            ret = my_strcat(cdepth, "(Parameter\n", cbody, cdepth, ")\n", VEND);
        }
        break;
    case Var_Node:
        sprintf(temp, "(Var \"%s\")\n", node->svalue);
        ret = my_strcat(cdepth, temp, VEND);
        break;
    case Num_Node:
        sprintf(temp, "(Num %lld)\n", node->ivalue);
        ret = my_strcat(cdepth, temp, VEND);
        break;
    case String_Node:
        ret = my_strcat(cdepth, "(String \"", node->svalue, "\")\n", VEND);
        break;
    case Add_Node:
        ret = my_strcat(cdepth, "(+\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Sub_Node:
        ret = my_strcat(cdepth, "(-\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Mul_Node:
        ret = my_strcat(cdepth, "(*\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Div_Node:
        ret = my_strcat(cdepth, "(/\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Mod_Node:
        ret = my_strcat(cdepth, "(%%\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Less_Node:
        ret = my_strcat(cdepth, "(<\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Eq_Node:
        ret = my_strcat(cdepth, "(==\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Or_Node:
        ret = my_strcat(cdepth, "(||\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case And_Node:
        ret = my_strcat(cdepth, "(&&\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Power_Node:
        ret = my_strcat(cdepth, "(**\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Not_Node:
        ret = my_strcat(cdepth, "(!\n", cleft, cright, cdepth, ")\n", VEND);
        break;
    case Uminus_Node:
        ret = my_strcat(cdepth, "(-\n", cleft, cright, cdepth, ")\n", VEND);
        break;

    default:
        sprintf(temp, "unknown node type: %d", node->type);
        run_exit(node, temp);
        break;
    }

    my_free_str(cdepth);
    my_free_str(cleft);
    my_free_str(cright);
    my_free_str(cbody);
    my_free_str(ccond);
    return ret;
}
void run_start(Node *node)
{
    set_parent_nodes(node);
    run(node, g_env);
}
Node *run(Node *node, Env *env)
{
    char *s1;
    Node *n1;
    Node *n2;
    Node *left;
    Node *right;
    int64_t i1;
    Env *e1;

    if (node == NULL)
        return (0);

    debug_check_map_node(node);

    switch (node->type)
    {
    case Stmt_List_Node:
        left = run(node->left, env);
        if (left && left->return_value)
        {
            return left;
        }
        right = run(node->right, env);
        return right;
        break;
    case Print_Val_Node:
        n1 = run(node->left, env);
        if (!n1)
        {
            return NULL;
        }

        if (n1->type == Num_Node)
        {
            printf("%lld", n1->ivalue);
        }
        else if (n1->type == String_Node)
        {
            printf("%s", n1->svalue);
        }
        else
        {
            s1 = node_tostring(n1, env, 0);
            printf("%s", s1);
        }
        break;
    case Println_Node:
        printf("\n");
        break;
    case Debug_Node:
        printf("\nDebugNode:\n");
        print_node(node->left, env);
        break;
    case If_Node:
        n1 = run(node->cond, env);
        if (n1->type != Num_Node)
        {
            run_exit(node, "if");
        }

        if (n1->ivalue)
        {
            left = run(node->left, env);

            my_free_node(n1, 0);

            return left;
        }
        else
        {
            right = run(node->right, env);

            my_free_node(n1, 0);

            return right;
        }
        break;
    case While_Node:
        n2 = 0;
        n1 = run(node->cond, env);
        if (n1->type != Num_Node)
        {
            run_exit(node, "while");
        }
        while (n1->ivalue)
        {
            n2 = run(node->body, env);
            if (n2 && n2->return_value)
            {
                return n2;
            }

            n1 = run(node->cond, env);
            if (n1->type != Num_Node)
            {
                run_exit(node, "while");
            }
        }
        return n2;
        break;
    case For_Node:
        n2 = 0;
        run(node->left, env);

        n1 = run(node->cond, env);
        if (n1->type != Num_Node)
        {
            run_exit(node, "for");
            exit(1);
        }
        while (n1->ivalue)
        {
            n2 = run(node->body, env);

            if (n2 && n2->return_value)
            {
                return n2;
            }

            run(node->right, env);

            n1 = run(node->cond, env);
            if (n1->type != Num_Node)
            {
                run_exit(node, "for");
                exit(1);
            }
        }
        return n2;
        break;
    case Assign_Node:
        n1 = run(node->left, env);
        n1->env_var = 1;

        assign_node_nonvar(env, node->svalue, n1);
        break;
    case Assign_Var_Node:
        n1 = run(node->left, env);
        n1->env_var = 1;

        cpp_map_set(map_stringkey_kind, env->map_void, node->svalue, n1);

        break;
    case Function_Node:
        return node;
        break;
    case Call_Node:
        n2 = 0;
        // 無名関数はない。svalueから変数取得。
        n1 = get_var(env, node->svalue);
        if (!n1)
        {
            sprintf(temp, "var %s not found", node->svalue);
            run_exit(node, temp);
        }

        if (n1->type != Function_Node && n1->type != Closure_Node)
        {
            sprintf(temp, "var %s not Function_Node nor Closure_Node", node->svalue);
            run_exit(node, temp);
        }

        if (n1->type == Closure_Node)
        {
            env = n1->env;
            n1 = n1->body;
        }

        // 変数(env)の階層構造を作る。
        e1 = make_env(env);

        call_assign_parameters(e1, n1, node);

        // 関数をcall
        n2 = run(n1->body, e1);
        if (!n2)
        {
            free_env(e1);
            return NULL;
        }

        // 	callの返り値が関数ならクロージャーにしてenvを保存する。
        if (n2->type == Function_Node)
        {
            n2 = make_node_closure(n2, e1);
        }
        else
        {
            free_env(e1);
        }

        n2->return_value = 0;
        return n2;
        break;
    case Return_Node:
        n2 = run(node->body, env);
        n2->return_value = 1;
        return n2;
        break;
    case Parameter_List_Node:
        printf("parameter list node\n");
        break;
    case Parameter_Node:
        printf("parameter node\n");
        break;
    case Var_Node:
        n1 = get_var(env, node->svalue);
        if (!n1)
        {
            sprintf(temp, "var %s not found\n", node->svalue);
            run_exit(node, temp);
        }
        return n1;
        break;
    case Num_Node:
        return node;
        break;
    case String_Node:
        return node;
        break;
    case Add_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue + right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Add");
        }
        break;
    case Sub_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue - right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Sub");
        }
        break;
    case Mul_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue * right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Mul");
        }
        break;
    case Div_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue / right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Div");
        }
        break;
    case Mod_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue % right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Mod");
        }
        break;
    case Less_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue < right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Less");
        }
        break;
    case Eq_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue == right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Eq");
        }
        break;
    case Or_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue || right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Or");
        }
        break;
    case And_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            n1 = make_node_num(left->ivalue && right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            return n1;
        }
        else
        {
            run_exit(node, "And");
        }
        break;
    case Power_Node:
        left = run(node->left, env);
        right = run(node->right, env);
        if (left->type == Num_Node && right->type == Num_Node)
        {
            i1 = my_power(left->ivalue, right->ivalue);

            my_free_node(left, 0);
            my_free_node(right, 0);

            n1 = make_node_num(i1);
            return n1;
        }
        else
        {
            run_exit(node, "Power");
        }
        break;
    case Not_Node:
        left = run(node->left, env);
        if (left->type == Num_Node)
        {
            n1 = make_node_num(!left->ivalue);

            my_free_node(left, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Not");
        }
        break;
    case Uminus_Node:
        left = run(node->left, env);
        if (left->type == Num_Node)
        {
            n1 = make_node_num(-left->ivalue);

            my_free_node(left, 0);

            return n1;
        }
        else
        {
            run_exit(node, "Not");
        }
        break;

    default:
        sprintf(temp, "unexpected node type: %d", node->type);
        run_exit(node, temp);
        break;
    }
    return 0;
}
