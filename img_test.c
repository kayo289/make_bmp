#include <fcntl.h>
#include "mlx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define IMG_WIDTH 800
#define IMG_HEIGHT 600

typedef struct s_img
{
	void *img_ptr;
	int *data;
	int size_l;
	int bpp;
	int endian;
} t_img;

typedef struct s_mlx
{
	void *mlx_ptr;
	void *win;
} t_mlx;

void build_header(char *bmp_header, uint32_t n_pixel)
{
	int i;

	i = 0;
	uint32_t file_size = n_pixel + 54;
	int offset = 54;
	int header_size = 40;
	int width = WIN_WIDTH;
	int height = WIN_HEIGHT;
	int planes = 1;
	int bpp = 32;
	memcpy(&bmp_header[i], "BM", 2);
	memcpy(&bmp_header[i += 2], &file_size, 4);
	memset(&bmp_header[i += 4], 0, 4);
	memcpy(&bmp_header[i += 4], &offset, 4);
	memcpy(&bmp_header[i += 4], &header_size, 4);
	memcpy(&bmp_header[i += 4], &width, 4);
	memcpy(&bmp_header[i += 4], &height, 4);
	memcpy(&bmp_header[i += 4], &planes, 2);
	memcpy(&bmp_header[i += 2], &bpp, 2);
	memset(&bmp_header[i += 2], 0, 24);
}

static void build_pixel_data(t_img *img, int *pixel_data)
{
	int i;
	int j;
	int k;

	i = IMG_HEIGHT - 1;
	k = 0;
	while (i >= 0)
	{
		j = 0;
		while (j < IMG_WIDTH)
		{
			pixel_data[i * IMG_WIDTH + j] = img->data[k++];
			j++;
		}
		i--;
	}
}

int main(void)
{
	t_mlx *mlx;
	int count_w;
	int count_h;
	t_img img;
	char bmp_header[54];
	int *pixel_data;
	uint32_t n_pixel;
	int fd;

	mlx->mlx_ptr = mlx_init();
	mlx->win = mlx_new_window(mlx->mlx_ptr, WIN_WIDTH, WIN_HEIGHT, "A simple example");
	img.img_ptr = mlx_new_image(mlx->mlx_ptr, IMG_WIDTH, IMG_HEIGHT);
	img.data = (int *)mlx_get_data_addr(img.img_ptr, &img.bpp, &img.size_l, &img.endian);

	count_h = 0;
	while (count_h < IMG_HEIGHT)
	{
		count_w = 0;
		while (count_w < IMG_WIDTH)
		{
			if (count_h <= IMG_HEIGHT / 2)
				img.data[count_h * IMG_WIDTH + count_w] = 0x89ffff;
			else
				img.data[count_h * IMG_WIDTH + count_w] = 33423360;
			count_w++;
		}
		count_h++;
	}
	fd = open("capture.bmp", O_CREAT | O_WRONLY, 0644);
	n_pixel = IMG_HEIGHT * IMG_WIDTH * 4;
	if (!(pixel_data = (int *)malloc(sizeof(int) * n_pixel)))
		exit(1);
	build_header(bmp_header, n_pixel);
	build_pixel_data(&img, pixel_data);
	write(fd, bmp_header, 54);
	write(fd, pixel_data, n_pixel);
	close(fd);
	free(pixel_data);
	mlx_put_image_to_window(mlx->mlx_ptr, mlx->win, img.img_ptr, 0, 0);
	mlx_loop(mlx->mlx_ptr);
	return (0);
}
