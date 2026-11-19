if (!localStorage.getItem('theme')) {
  localStorage.setItem('theme', 'dark');
  document.body.dataset.theme = 'dark';
}
