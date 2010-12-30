using System;
using System.Diagnostics;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Security.Permissions;
using OpenNI.Properties;

namespace OpenNI
{
    /// <summary>
    /// Represents errors that occur inside an OpenNI function call.
    /// </summary>
    [Serializable]
    [ComVisible(true)]
    public sealed class OpenNiException
        : Exception
    {
        [NonSerialized]
        private OpenNiExceptionData data;

        /// <summary>
        /// Initializes a new OpenNi.OpenNiException.
        /// </summary>
        public OpenNiException() 
            : base(Resources.OpenNiException_DefaultMessage) 
        {
        }

        /// <summary>
        /// Initializes a new OpenNi.OpenNiException.
        /// </summary>
        /// <param name="message">The error message that explains the reason for the exception.</param>
        public OpenNiException(string message) 
            : base(message) 
        {
        }

        /// <summary>
        /// Initializes a new OpenNi.OpenNiException.
        /// </summary>
        /// <param name="message">The error message that explains the reason for the exception.</param>
        /// <param name="innerException">
        /// The exception that is the cause of the iterator exception, or a null reference
        /// (Nothing in Visual Basic) if no inner exception is specified.
        /// </param>
        public OpenNiException(string message, Exception innerException) 
            : base(message, innerException) 
        {
        }

        /// <summary>
        /// Initializes a new OpenNi.OpenNiException.
        /// </summary>
        /// <param name="status">The OpenNI error code.</param>
        internal OpenNiException(Status status)
            : base(Resources.OpenNiException_DefaultMessage)
        {
            this.data.ErrorName = Status.GetName(status);
            this.data.ErrorDescription = Status.GetString(status);

            this.SerializeObjectState += (exception, eventArgs) => eventArgs.AddSerializedState(data);
        }

        /// <summary>
        /// The OpenNI error name.
        /// </summary>
        public string ErrorName
        {
            get
            {
                return this.data.ErrorName;
            }
        }

        /// <summary>
        /// The OpenNI error Description.
        /// </summary>
        public string ErrorDescription
        {
            get
            {
                return this.data.ErrorDescription;
            }
        }

        /// <summary>
        /// Gets a message that describes the iterator exception.
        /// </summary>
        public override string Message
        {
            get
            {
                var message = new System.Text.StringBuilder();

                if (String.IsNullOrEmpty(base.Message)) 
                    message.Append(Resources.OpenNiException_DefaultMessage);
                else 
                    message.Append(base.Message);
                message.Append(Environment.NewLine);

                if (!string.IsNullOrWhiteSpace(this.data.ErrorName))
                {
                    message.Append(Resources.OpenNiException_ErrorName);
                    message.Append(this.data.ErrorName);
                    message.Append(Environment.NewLine);
                }
                if (!string.IsNullOrWhiteSpace(this.data.ErrorDescription))
                {
                    message.Append(Resources.OpenNiException_ErrorDescription);
                    message.Append(this.data.ErrorDescription);
                    message.Append(Environment.NewLine);
                }

                return message.ToString();
            }
        }

        #region OpenNiExceptionData implementation

        [Serializable]
        private struct OpenNiExceptionData
            : ISafeSerializationData
        {
            public string ErrorName { get; set; }
            public string ErrorDescription { get; set; }

            void ISafeSerializationData.CompleteDeserialization(object deserialized)
            {
                var exception = deserialized as OpenNiException;
                exception.data = this;
            }
        }

        #endregion
    }
}
