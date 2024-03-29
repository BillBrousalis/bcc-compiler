#include "include/util.h"

char *format(AST_t *node)
{
  size_t sz = 256;
  char *buf = calloc(sz, sizeof(char));
  snprintf(buf, sz, "[%ld] ", node->node_id);
  switch (node->node_type) {
    case AST_NOP: strcat(buf, "NOP,"); break;
    case AST_NUM: snprintf(buf+strlen(buf), sz, "NUM: %ld,", node->num_value); break;
    case AST_ID: snprintf(buf+strlen(buf), sz, "ID: %s,", node->name); break;
    case AST_CALL: snprintf(buf+strlen(buf), sz, "CALL: %s(),", node->name); break;
    case AST_COND: snprintf(buf+strlen(buf), sz, "COND: %s,", op_to_str(node->op)); break;
    case AST_BINOP: snprintf(buf+strlen(buf), sz, "BINOP: %s,", op_to_str(node->op)); break;
    case AST_FUNCTION: snprintf(buf+strlen(buf), sz, "FUNCTION: %s %s,", type_to_str(node->specs_type), node->name); break;
    case AST_RETURN: strcat(buf, "RETURN,"); break;
    case AST_DECL: snprintf(buf+strlen(buf), sz, "DECL: %s %s,", type_to_str(node->specs_type), node->name); break;
    case AST_COMPOUND: strcat(buf, "COMPOUND,"); break;
    case AST_IF: strcat(buf, "IF:,"); break;
    case AST_WHILE: strcat(buf, "WHILE:,"); break;
    case AST_FOR: strcat(buf, "FOR:,"); break;
    case AST_ASSIGNMENT: snprintf(buf+strlen(buf), sz, "ASSIGNMENT: %s,", node->name); break;
    default:
      printf("node type = %s\n", AST_type_to_str(node->node_type));
      error_exit("format() - default reached\n");
  }
  return buf;
}

char *format_annot(AST_t *paren, AST_t *child, char *annot)
{
  size_t sz = 1024;
  char *buf = calloc(sz, sizeof(char));
  snprintf(buf, sz, "%s%s->%s\n", format(paren), format(child), annot);
  return buf;
}

// format AST in a way that can be used by plot.py
// TODO: clean up eventually
void make_ast_graph(AST_t *root)
{
  assert(root != NULL && "create_ast_file - root is NULL");
  List_t *list = init_list(sizeof(char *));   // data to write to file
  List_t *queue = init_list(sizeof(AST_t *)); // queue to iterate on
  List_t *annot = init_list(sizeof(char *));  // annotations for arrows
  list_push(queue, root);
  while (queue->size) {
    AST_t *node = (AST_t *)queue->items[0];
    list_push(list, format(node));
    list_pop_first(queue);
    if (node == NULL) continue;
    switch (node->node_type) {
      // terminals
      case AST_NOP:
      case AST_NUM:
      case AST_ID:
      case AST_DECL:
        break;
      case AST_CALL:
        for (int i = 0; i < node->args->size; ++i) {
          list_push(list, format(node->args->items[i]));
          list_push(annot, format_annot(node, node->args->items[i], "arg"));
          list_push(queue, node->args->items[i]);
        }
        break;
      case AST_COND:
      case AST_BINOP:
        list_push(list, format(node->left));
        list_push(annot, format_annot(node, node->left, "left"));
        list_push(list, format(node->right));
        list_push(annot, format_annot(node, node->right, "right"));
        list_push(queue, node->left);
        list_push(queue, node->right);
        break;
      case AST_FUNCTION:
        // params
        for (int i = 0; i < node->params->size; ++i) {
          list_push(list, format(node->params->items[i]));
          list_push(annot, format_annot(node, node->params->items[i], "param"));
          list_push(queue, node->params->items[i]);
        }
        // body
        list_push(list, format(node->body));
        list_push(annot, format_annot(node, node->body, "body"));
        list_push(queue, node->body);
        break;
      case AST_COMPOUND:
        for (int i = 0; i < node->children->size; ++i) {
          list_push(list, format(node->children->items[i]));
          list_push(annot, format_annot(node, node->children->items[i], "child"));
          list_push(queue, node->children->items[i]);
        }
        break;
      case AST_RETURN:
        list_push(list, format(node->value));
        list_push(annot, format_annot(node, node->value, "value"));
        list_push(queue, node->value);
        break;
      case AST_ASSIGNMENT:
        if (node->decl != NULL) {
          list_push(list, format(node->decl));
          list_push(annot, format_annot(node, node->decl, "decl"));
          list_push(queue, node->decl);
        }
        list_push(list, format(node->value));
        list_push(annot, format_annot(node, node->value, "value"));
        list_push(queue, node->value);
        break;
      case AST_IF:
        list_push(list, format(node->cond));
        list_push(annot, format_annot(node, node->cond, "cond"));
        list_push(queue, node->cond);
        list_push(list, format(node->ifbody));
        list_push(annot, format_annot(node, node->ifbody, "ifbody"));
        list_push(queue, node->ifbody);
        list_push(list, format(node->elsebody));
        list_push(annot, format_annot(node, node->elsebody, "elsebody"));
        list_push(queue, node->elsebody);
        break;
      case AST_WHILE:
        list_push(list, format(node->cond));
        list_push(annot, format_annot(node, node->cond, "cond"));
        list_push(queue, node->cond);
        list_push(list, format(node->body));
        list_push(annot, format_annot(node, node->body, "body"));
        list_push(queue, node->body);
        break;
      case AST_FOR:
        list_push(list, format(node->stmt_initializer));
        list_push(annot, format_annot(node, node->stmt_initializer, "stmt_initializer"));
        list_push(queue, node->stmt_initializer);
        list_push(list, format(node->cond));
        list_push(annot, format_annot(node, node->cond, "cond"));
        list_push(queue, node->cond);
        list_push(list, format(node->stmt_update));
        list_push(annot, format_annot(node, node->stmt_update, "stmt_update"));
        list_push(queue, node->stmt_update);
        list_push(list, format(node->body));
        list_push(annot, format_annot(node, node->body, "body"));
        list_push(queue, node->body);
        break;
      default: error_exit("create_ast_file() - unknown node type\n");
    }
    list_push(list, "\n");
  }

  char *path = "pyutil/AST_data.txt";
  FILE *fp;
  if ((fp = fopen(path, "w")) == NULL) {
    printf("Error opening %s\n", path);
    exit(1);
  }

  // tree
  for (int i = 0; i < list->size; ++i)
    fwrite(list->items[i], sizeof(char), strlen(list->items[i]), fp);

  // annotations
  char *separator = "Annotations:\n";
  fwrite(separator, sizeof(char), strlen(separator), fp);
  for (int i = 0; i < annot->size; ++i)
    fwrite(annot->items[i], sizeof(char), strlen(annot->items[i]), fp);
  
  fclose(fp);

  // use python script
  system("cd pyutil/ && ./plot.py");
}
