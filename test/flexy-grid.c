#include <endless/endless.h>

static const char *icons[] = {
  "applications-accessories",
  "applications-development",
  "applications-engineering",
  "applications-games",
  "applications-graphics",
  "applications-internet",
  "applications-multimedia",
  "applications-office",
  "applications-other",
  "applications-science",
  "applications-system",
  "applications-utilities",
};

#define CHAR(r,g,b) { r / 255.0, g / 255.0, b / 255.0, 1.0 }

static GdkRGBA colors[] = {
  CHAR (0xff, 0x00, 0x00),
  CHAR (0x80, 0x00, 0x00),
  CHAR (0x00, 0xff, 0x00),
  CHAR (0x00, 0x80, 0x00),
  CHAR (0x00, 0x00, 0xff),
  CHAR (0x00, 0x00, 0x80),
  CHAR (0x00, 0xff, 0xff),
  CHAR (0x00, 0x80, 0x80),
  CHAR (0xff, 0x00, 0xff),
  CHAR (0x80, 0x00, 0x80),
  CHAR (0xff, 0xff, 0x00),
  CHAR (0x80, 0x80, 0x00),
  CHAR (0xa0, 0xa0, 0xa4),
  CHAR (0x80, 0x80, 0x80),
  CHAR (0xc0, 0xc0, 0xc0),
};

static EosFlexyShape shapes[] = {
  EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL,
  EOS_FLEXY_SHAPE_MEDIUM_VERTICAL,
  EOS_FLEXY_SHAPE_SMALL,
  EOS_FLEXY_SHAPE_SMALL,
  EOS_FLEXY_SHAPE_SMALL,
  EOS_FLEXY_SHAPE_MEDIUM_VERTICAL,
  EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL,
  EOS_FLEXY_SHAPE_SMALL,
};

static void
on_cell_selected (EosFlexyGrid *grid, EosFlexyGridCell *cell)
{
  g_print ("Cell %p selected\n", cell);
}

static void
on_cell_activated (EosFlexyGrid *grid, EosFlexyGridCell *cell)
{
  g_print ("Cell %p activated\n", cell);
}

int
main (int argc, char *argv[])
{
  gtk_init (NULL, NULL);

  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show (window);

  GtkWidget *scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (window), scroll);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_NEVER,
                                  GTK_POLICY_AUTOMATIC);
  gtk_widget_show (scroll);

  GtkWidget *grid = eos_flexy_grid_new ();
  g_signal_connect (grid, "cell-selected", G_CALLBACK (on_cell_selected), NULL);
  g_signal_connect (grid, "cell-activated", G_CALLBACK (on_cell_activated), NULL);
  gtk_container_add (GTK_CONTAINER (scroll), grid);
  gtk_widget_show (grid);

  for (guint i; i < 8; i++)
    {
      GtkWidget *cell = eos_flexy_grid_cell_new ();
      eos_flexy_grid_cell_set_shape (EOS_FLEXY_GRID_CELL (cell), shapes[i]);
      gtk_container_add (GTK_CONTAINER (grid), cell);
      gtk_widget_show (cell);

      GtkWidget *frame = gtk_event_box_new ();
      gtk_widget_override_background_color (frame, GTK_STATE_FLAG_NORMAL, &colors[i]);
      gtk_container_add (GTK_CONTAINER (cell), frame);
      gtk_widget_show (frame);

      GtkWidget *image = gtk_image_new ();
      gtk_image_set_from_icon_name (GTK_IMAGE (image), icons[i], GTK_ICON_SIZE_DIALOG);
      gtk_container_add (GTK_CONTAINER (frame), image);
      gtk_widget_set_hexpand (image, TRUE);
      gtk_widget_set_vexpand (image, TRUE);
      gtk_widget_show (image);
    }

  gtk_main ();

  return 0;
}
