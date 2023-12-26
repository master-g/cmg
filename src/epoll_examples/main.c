//
// Created by MasterG on 2020/5/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 5

int main(int argc, char **argv) {
  int epoll_fd, timer_fd;
  struct epoll_event event, events[MAX_EVENTS];

  // 创建一个epoll实例
  epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }
  printf("epoll_fd: %d\n", epoll_fd);

  // 创建 timerfd 实例
  timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (timer_fd == -1) {
    perror("timerfd_create");
    exit(EXIT_FAILURE);
  }
  printf("timer_fd: %d\n", timer_fd);

  // 设置定时器的超时和间隔时间
  struct itimerspec timer_spec;
  timer_spec.it_value.tv_sec = 5; // 初始超时时间 5s
  timer_spec.it_value.tv_nsec = 0;
  timer_spec.it_interval.tv_sec = 1; // 间隔时间 1s
  timer_spec.it_interval.tv_nsec = 0;

  // 定时器，启动！
  if (timerfd_settime(timer_fd, 0, &timer_spec, NULL) == -1) {
    perror("timerfd_settime");
    exit(EXIT_FAILURE);
  }
  printf("timer_fd started!\n");

  // 将 timer_fd 加入到 epoll 监听列表中
  event.events = EPOLLIN;
  event.data.fd = timer_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &event) == -1) {
    perror("epoll_ctl");
    exit(EXIT_FAILURE);
  }
  printf("timer_fd added to epoll_fd!\n");

  // 开始监听
  while (1) {
    int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_events; ++i) {
      if (events[i].data.fd == timer_fd) {
        printf("tick!\n");
        uint64_t exp;
        ssize_t s = read(timer_fd, &exp, sizeof(uint64_t));
        if (s != sizeof(uint64_t)) {
          perror("read");
          exit(EXIT_FAILURE);
        }
      }
    }
  }

  // 关闭文件描述符
  close(timer_fd);
  close(epoll_fd);
}
