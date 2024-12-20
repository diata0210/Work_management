// src/client/ui/main_window.c

#include <gtk/gtk.h>
#include "../include/client.h"
#include "project_view.h"
#include "chat_window.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *projects_tree;
    GtkWidget *tasks_tree;
    GtkWidget *chat_list;
    GtkWidget *notification_area;
    GtkWidget *status_bar;
    Client *client;
} MainWindow;

static void create_header_bar(MainWindow *main_window) {
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Project Management System");

    // Add new project button
    GtkWidget *new_project_button = gtk_button_new_from_icon_name(
        "document-new", GTK_ICON_SIZE_BUTTON);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), new_project_button);

    // Add settings button
    GtkWidget *settings_button = gtk_button_new_from_icon_name(
        "preferences-system", GTK_ICON_SIZE_BUTTON);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), settings_button);

    gtk_window_set_titlebar(GTK_WINDOW(main_window->window), header);
}

static void create_projects_view(MainWindow *main_window) {
    GtkWidget *frame = gtk_frame_new("Projects");
    GtkListStore *store = gtk_list_store_new(4, 
        G_TYPE_STRING,  // Project name
        G_TYPE_STRING,  // Status
        G_TYPE_INT,     // Progress
        G_TYPE_INT      // Project ID
    );

    main_window->projects_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    // Add columns
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Project", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->projects_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Status", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->projects_tree), column);

    renderer = gtk_cell_renderer_progress_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Progress", renderer, "value", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->projects_tree), column);

    gtk_container_add(GTK_CONTAINER(frame), main_window->projects_tree);
}

static void create_tasks_view(MainWindow *main_window) {
    GtkWidget *frame = gtk_frame_new("My Tasks");
    GtkListStore *store = gtk_list_store_new(5,
        G_TYPE_STRING,  // Task name
        G_TYPE_STRING,  // Project
        G_TYPE_STRING,  // Deadline
        G_TYPE_STRING,  // Status
        G_TYPE_INT      // Task ID
    );

    main_window->tasks_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    // Add columns
    const char *titles[] = {"Task", "Project", "Deadline", "Status"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tasks_tree), column);
    }

    gtk_container_add(GTK_CONTAINER(frame), main_window->tasks_tree);
}

static void create_chat_list(MainWindow *main_window) {
    GtkWidget *frame = gtk_frame_new("Chats");
    GtkListStore *store = gtk_list_store_new(3,
        G_TYPE_STRING,  // Contact name
        G_TYPE_STRING,  // Last message
        G_TYPE_INT      // Chat ID
    );

    main_window->chat_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Contacts", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->chat_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Last Message", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->chat_list), column);

    gtk_container_add(GTK_CONTAINER(frame), main_window->chat_list);
}

static void create_notification_area(MainWindow *main_window) {
    GtkWidget *frame = gtk_frame_new("Notifications");
    main_window->notification_area = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(frame), main_window->notification_area);
}

static void on_project_row_activated(GtkTreeView *tree_view, 
                                   GtkTreePath *path,
                                   GtkTreeViewColumn *column,
                                   gpointer user_data) {
    MainWindow *main_window = (MainWindow*)user_data;
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gint project_id;
        gtk_tree_model_get(model, &iter, 3, &project_id, -1);
        
        // Open project view
        Project *project = get_project_by_id(main_window->client, project_id);
        if (project) {
            create_project_view(main_window->client, project);
        }
    }
}

static void on_chat_row_activated(GtkTreeView *tree_view,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                                gpointer user_data) {
    MainWindow *main_window = (MainWindow*)user_data;
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gint chat_id;
        gchar *contact_name;
        gtk_tree_model_get(model, &iter, 
                          0, &contact_name,
                          2, &chat_id, 
                          -1);
        
        // Open chat window
        create_chat_window(main_window->client, chat_id, contact_name);
        g_free(contact_name);
    }
}

MainWindow* create_main_window(Client *client) {
    MainWindow *main_window = g_new0(MainWindow, 1);
    main_window->client = client;

    // Create main window
    main_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window->window), "Project Management System");
    gtk_window_set_default_size(GTK_WINDOW(main_window->window), 1024, 768);

    // Create header bar
    create_header_bar(main_window);

    // Create main layout
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create views
    create_projects_view(main_window);
    create_tasks_view(main_window);
    create_chat_list(main_window);
    create_notification_area(main_window);

    // Create status bar
    main_window->status_bar = gtk_statusbar_new();

    // Pack widgets
    gtk_box_pack_start(GTK_BOX(left_panel), main_window->projects_tree, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(left_panel), main_window->tasks_tree, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(right_panel), main_window->chat_list, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(right_panel), main_window->notification_area, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(content_box), left_panel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_box), right_panel, FALSE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), main_window->status_bar, FALSE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(main_window->window), main_box);

    // Connect signals
    g_signal_connect(G_OBJECT(main_window->projects_tree), "row-activated",
                    G_CALLBACK(on_project_row_activated), main_window);
    g_signal_connect(G_OBJECT(main_window->chat_list), "row-activated",
                    G_CALLBACK(on_chat_row_activated), main_window);
    g_signal_connect(G_OBJECT(main_window->window), "destroy",
                    G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(main_window->window);
    return main_window;
}

void update_projects_list(MainWindow *main_window, GList *projects) {
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(main_window->projects_tree)));
    gtk_list_store_clear(store);

    GList *p;
    for (p = projects; p != NULL; p = p->next) {
        Project *project = (Project*)p->data;
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, project->name,
                          1, project->status,
                          2, project->progress,
                          3, project->id,
                          -1);
    }
}
