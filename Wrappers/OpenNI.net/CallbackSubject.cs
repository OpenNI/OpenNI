using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenNI
{
    internal class CallbackSubject<T>
        : IObservable<T>
        , IObserver<T>
    {
        private readonly IObservable<T> observable;
        private readonly List<IObserver<T>> observers = new List<IObserver<T>>();

        public CallbackSubject(Action registerCallback, Action unregisterCallback)
        {
            this.observable = Observable.Create<T>(observer =>
            {
                lock(this.observers)
                {
                    // register the callback if required
                    if (this.observers.Count == 0)
                    {
                        registerCallback();
                    }

                    // add to the observers list
                    this.observers.Add(observer);
                }

                // return the unregister method
                return () =>
                {
                    lock (this.observers)
                    {
                        // remove from the observers list
                        this.observers.Remove(observer);

                        // unregister the callback if possible
                        if (this.observers.Count == 0)
                        {
                            unregisterCallback();
                        }
                    }
                };
            });
        }

        public IDisposable Subscribe(IObserver<T> observer)
        {
            return this.observable.Subscribe(observer);
        }

        IDisposable IObservable<T>.Subscribe(IObserver<T> observer)
        {
            return this.observable.Subscribe(observer);
        }

        public void OnCompleted()
        {
            lock (this.observers)
            {
                this.observers.ForEach(observer => observer.OnCompleted());
            }
        }

        public void OnError(Exception error)
        {
            lock (this.observers)
            {
                this.observers.ForEach(observer => observer.OnError(error));
            }
        }

        public void OnNext(T value)
        {
            lock (this.observers)
            {
                this.observers.ForEach(observer => observer.OnNext(value));
            }
        }
    }
}
