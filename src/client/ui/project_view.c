// src/client/ui/project_view.c

#include <gtk/gtk.h>
#include "../include/client.h"
#include "../include/project.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *tasks_tree;
    GtkWidget *members_list;
    GtkWidget *chat_view;
    GtkWidget *files_list;
    GtkNotebook *notebook;
    Project *project;
    Client *client;
} ProjectView;

static void create_tasks_page(ProjectView *view) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    GtkToolItem *new_task = gtk_tool_button_new(NULL, "New Task");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_task, -1);
    
    // Create tree view
    GtkListStore *store = gtk_list_store_new(6,
        G_TYPE_STRING,  // Task name
        G_TYPE_STRING,  // Assigned to
        G_TYPE_STRING,  // Status
        G_TYPE_STRING,  // Due date
        G_TYPE_INT,     // Progress
        G_TYPE_INT      // Task ID
    );
    
    view->tasks_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    const char *titles[] = {"Task", "Assigned To", "Status", "Due Date", "Progress"};
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        
        if (i == 4) {
            renderer = gtk_cell_renderer_progress_new();
            column = gtk_tree_view_column_new_with_attributes(
                titles[i], renderer, "value", i, NULL);
        } else {
            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(
                titles[i], renderer, "text", i, NULL);
        }
        
        gtk_tree_view_append_column(GTK_TREE_VIEW(view->tasks_tree), column);
    }
    
    // Pack widgets
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), view->tasks_tree);
    
    gtk_box_pack_start(GTK_BOX(page), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);
    
    gtk_notebook_append_page(view->notebook, page, 
                           gtk_label_new("Tasks"));
}

static void create_members_page(ProjectView *view) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    GtkToolItem *add_member = gtk_tool_button_new(NULL, "Add Member");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), add_member, -1);
    
    // Create list view
    GtkListStore *store = gtk_list_store_new(3,
        G_TYPE_STRING,  // Username
        G_TYPE_STRING,  // Role
        G_TYPE_INT      // User ID
    );
    
    view->members_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Member", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view->members_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Role", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view->members_list), column);
    
    // Pack widgets
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), view->members_list);
    
    gtk_box_pack_start(GTK_BOX(page), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);
    
    gtk_notebook_append_page(view->notebook, page,
                           gtk_label_new("Members"));
}

static void create_chat_page(ProjectView *view) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create chat view
    view->chat_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view->chat_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->chat_view), GTK_WRAP_WORD_CHAR);
    
    // Create message entry
    GtkWidget *entry = gtk_entry_new();
    GtkWidget *send_button = gtk_button_new_with_label("Send");
    
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(input_box), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), send_button, FALSE, FALSE, 0);
    
    // Pack widgets
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), view->chat_view);
    
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), input_box, FALSE, FALSE, 0);
    
    gtk_notebook_append_page(view->notebook, page,
                           gtk_label_new("Chat"));
}

static void create_files_page(ProjectView *view) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    GtkToolItem *upload_file = gtk_tool_button_new(NULL, "Upload File");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), upload_file, -1);
    
    // Create files list
    GtkListStore *store = gtk_list_store_new(4,
        G_TYPE_STRING,  // Filename
        G_TYPE_STRING,  // Uploaded by
        G_TYPE_STRING,  // Upload date
        G_TYPE_INT      // File ID
    );
    
    view->files_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    const char *titles[] = {"File", "Uploaded By", "Date"};
    for (int i = 0; i < 3; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(view->files_list), column);
    }
    
    // Pack widgets
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), view->files_list);
    
    gtk_box_pack_start(GTK_BOX(page), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);
    
    gtk_notebook_append_page(view->notebook, page,
                           gtk_label_new("Files"));
}

ProjectView* create_project_view(Client *client, Project *project) {
    ProjectView *view = g_new0(ProjectView, 1);
    view->client = client;
    view->project = project;

    // Create main window
    view->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(view->window),
                        g_strdup_printf("Project: %s", project->name));
    gtk_window_set_default_size(GTK_WINDOW(view->window), 800, 600);

    // Create notebook
    view->notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_scrollable(view->notebook, TRUE);

    // Create pages
    create_tasks_page(view);
    create_members_page(view);
    create_chat_page(view);
    create_files_page(view);

    // Add notebook to window
    gtk_container_add(GTK_CONTAINER(view->window), GTK_WIDGET(view->notebook));

    gtk_widget_show_all(view->window);
    return view;
}

void update_project_view(ProjectView *view) {
    // Update tasks list
    GtkListStore *tasks_store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(view->tasks_tree)));
    gtk_list_store_clear(tasks_store);
    
    for (GList *t = view->project->tasks; t != NULL; t = t->next) {
        Task *task = (Task*)t->data;
        GtkTreeIter iter;
        gtk_list_store_append(tasks_store, &iter);
        gtk_list_store_set(tasks_store, &iter,
                          0, task->name,
                          1, task->assigned_to,
                          2, task->status,
                          3, task->due_date,
                          4, task->progress,
                          5, task->id,
                          -1);
    }
    
    // Update members list
    GtkListStore *members_store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(view->members_list)));
    gtk_list_store_clear(members_store);
    
    for (GList *m = view->project->members; m != NULL; m = m->next) {
        ProjectMember *member = (ProjectMember*)m->data;
        GtkTreeIter iter;
        gtk_list_store_append(members_store, &iter);
        gtk_list_store_set(members_store, &iter,
                          0, member->username,
                          1, member->role,
                          2, member->user_id,
                          -1);
    }
    
    // Update files list
    GtkListStore *files_store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(view->files_list)));
    gtk_list_store_clear(files_store);
    
    for (GList *f = view->project->files; f != NULL; f = f->next) {
        ProjectFile *file = (ProjectFile*)f->data;
        GtkTreeIter iter;
        gtk_list_store_append(files_store, &iter);
        gtk_list_store_set(files_store, &iter,
                          0, file->filename,
                          1, file->uploaded_by,
                          2, file->upload_date,
                          3, file->id,
                          -1);
    }
}
